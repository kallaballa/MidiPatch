
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <csignal>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mutex>

#include "cxxopts.hpp"
#include "RtAudio.h"
#include "RtMidi.h"
#include "RtError.h"
#include "lua.h"
#include "miby.h"

#include "logger.hpp"
#include "PatchScript.hpp"
#include "websocket.hpp"
#include "websocket_callbacks.hpp"
#include "pscript_singleton.hpp"

using namespace Tonic;
using namespace midipatch;

enum ExitCodes {
	PATCH_SCRIPT_ERROR = 1, OPTIONS_ERROR = 2, UNKNOWN_ERROR = 3
};

const unsigned int audio_channels = 2;

int8_t current_channel = -1;
uint8_t current_program = 127;
size_t controlNumberOffset = 0;

std::mutex midiMutex;
patchscript::PatchScript* pscript = nullptr;

int renderCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime,
		RtAudioStreamStatus status, void *userData) {
	if (Websocket::isInitialized() && Websocket::getInstance()->isRestartRequested()) {
		return 1;
	} else {
		pscript->fill((float*) outputBuffer, nBufferFrames, audio_channels);
		size_t lenBuf = nBufferFrames * audio_channels;
		std::vector<float> samples(lenBuf, 0);

		for (size_t i = 0; i < lenBuf; ++i) {
			samples[i] = ((float*) outputBuffer)[i];
		}
		if (Websocket::isInitialized())
			Websocket::getInstance()->sendAudio(samples);
	}
	return 0;
}

//
// MIDI Callbacks
//
// NOTE: these are brokered through the Miby parser, for which a state is
// instantiated for all configured MIDI inputs
//
void MIDIPATCH_Control_Change(miby_this_t a_miby) {
	PatchScript* pscript = PScriptsSingleton::getInstance();
	std::cout << "MIDIPATCH_Control_Change C:" << int(MIBY_CHAN(a_miby)) << " CC1: "
				<< int(MIBY_ARG0(a_miby)) << " CC2: " << int(MIBY_ARG1(a_miby)) << std::endl;

	if(current_channel != -1 && MIBY_CHAN(a_miby) != current_channel)
		return;
	std::vector<string> commonParams;

	//try to set a common parameter for all synths. NOTE: only works if all synthesizers have the same public parameters
	if (!pscript->getPolySynth()->getVoices().empty()) {
		Synth s;
		std::vector<string> currentParams;
		bool first = true;

			//check if all parameters match between all synths
		for (auto& pv : pscript->getPolySynth()->getVoices()) {
			currentParams.clear();
			s = pv.synth;
			for (auto& cp : s.getParameters()) {
				const string name = cp.getDisplayName();
					//Parameters with a name starting with '_' are private
				if (!name.empty() && name.at(0) != '_') {
					currentParams.push_back(name);
				}
			}
			if (first) {
				commonParams = currentParams;
				first = false;
			} else {
				if (currentParams != commonParams) {
					Logger::getInstance().warn("Synth parameters differ, can't set parameters globally");
					break;
				}
			}
		}

		std::vector<string> publicParameters = currentParams;
		string parent;
		string child;
		for (auto& pv : pscript->getPolySynth()->getVoices()) {
			s = pv.synth;

			// Note: MIBY_ARG0 = CC1, MIBY_ARG1 = CC2
			if (((uint8_t)MIBY_ARG0(a_miby) - controlNumberOffset) < publicParameters.size()) {
				const string& name = publicParameters[(uint8_t)MIBY_ARG0(a_miby) - controlNumberOffset];

				auto delim = name.find(".");

				if (delim != string::npos) {
					parent = name.substr(0, delim);
					child = name.substr(delim + 1);
				} else {
					parent = "Global";
					child = name;
				}
				s.setParameter(name, (float) MIBY_ARG0(a_miby) / 127.0);
				if (Websocket::isInitialized())
					Websocket::getInstance()->updateParameter(name, (float) MIBY_ARG0(a_miby) / 127.0);
			}
		}

	}
}

void MIDIPATCH_Note_On(miby_this_t a_miby) {
	PatchScript* pscript = PScriptsSingleton::getInstance();
	std::cout << "MIDIPATCH_Note_On C: " << (int)MIBY_CHAN(a_miby)
				<< " N: " << (int)MIBY_ARG0(a_miby) << " V: " << (int)MIBY_ARG1(a_miby) << std::endl;
	if(current_channel != -1 && MIBY_CHAN(a_miby) != current_channel)
		return;

	pscript->getPolySynth()->noteOn((uint8_t)MIBY_ARG0(a_miby), (uint8_t)MIBY_ARG1(a_miby));

	if (Websocket::isInitialized())
		Websocket::getInstance()->sendNoteOn((int)MIBY_ARG0(a_miby), (int)MIBY_ARG1(a_miby));
}

void MIDIPATCH_Note_Off(miby_this_t a_miby) {
	PatchScript* pscript = PScriptsSingleton::getInstance();
	std::cout << "MIDIPATCH_Note_Off C:" << (int)MIBY_CHAN(a_miby) << std::endl;
	if(current_channel != -1 && MIBY_CHAN(a_miby) != current_channel)
		return;

	pscript->getPolySynth()->noteOff((int)MIBY_ARG0(a_miby));

	if (Websocket::isInitialized())
		Websocket::getInstance()->sendNoteOff((int)MIBY_ARG0(a_miby));
}

void MIDIPATCH_Start(miby_this_t a_miby) {
	std::cout << "MIDIPATCH_Start." << std::endl;
}

void MIDIPATCH_Stop(miby_this_t a_miby) {
	std::cout << "MIDIPATCH_Stop." << std::endl;
}

void MIDIPATCH_Program_Change(miby_this_t a_miby) {
	std::cout << "MIDIPATCH_Program_Change C: " << (int)MIBY_CHAN(a_miby)
				<< " P: " << (int)MIBY_ARG0(a_miby) << std::endl;
	if(current_channel != -1 && MIBY_CHAN(a_miby) != current_channel)
		return;

	current_program = (int)MIBY_ARG0(a_miby);
}


//
// main entrypoint for MIDI messages
//
void mainMIDICallback(double deltatime, vector<unsigned char>* msg, void* userData) {
	std::unique_lock lock(midiMutex);
	if (Websocket::isInitialized() && Websocket::getInstance()->isRestartRequested())
		return;

	for(auto& inb: *msg) {
		// std::cout << " inb: " << inb << std::endl;
		miby_parse((miby_t *)userData, inb);
	}
}

int main(int argc, char ** argv) {
	std::string appName = argv[0];
	std::vector<int> midiIndex;
	int audioIndex = 0;
	unsigned int sampleRate;
	unsigned int bufferFrames;
	size_t port;
	string patchFile;
	string logFile;
	string bankFile;
	size_t numVoices;
	int8_t channel;
	const char* home = std::getenv(DEFAULT_ENV_VARIABLE);
	if(!home) {
		std::cerr << "Can't find environment variable: " << DEFAULT_ENV_VARIABLE << std::endl;
		exit(PATCH_SCRIPT_ERROR);
	}

	cxxopts::Options options(appName, "A scriptable, modular and real-time MIDI synthesizer");
	options.add_options()("h,help", "Print help messages")
			("m,midi", "The indices of the midi input ports to use.",
				cxxopts::value<std::vector<int>>(midiIndex)->default_value("0"))
			("a,audio",	"The index of the audio output port to use.",
					cxxopts::value<int>(audioIndex)->default_value("0"))
			("r,rate", "The audio output sample rate.",
					cxxopts::value<unsigned int>(sampleRate)->default_value("48000"))
			("b,buffer", "Number of frames per buffer.",
					cxxopts::value<unsigned int>(bufferFrames)->default_value("512"))
			("w,websocket", "The port number of the websocket server.",
					cxxopts::value<size_t>(port)->default_value("8080"))
			("o,offset", "The control number offset for parameter mapping",
				cxxopts::value<size_t>(controlNumberOffset)->default_value("52"))
			("v,voices", "The number of voices to run",
					cxxopts::value<size_t>(numVoices)->default_value("8"))
			("c,channel", "The midi channel to listen to or '-1' for all channels",
					cxxopts::value<int8_t>(channel)->default_value("-1"))
			("p,patchFile",	"The lua patchFile to use for the voices",
					cxxopts::value<string>(patchFile)->default_value(string(home) + "/" + DEFAULT_DATA_DIR + "/midipatch.pat"))
			("f,logFile", "The file to log to",
					cxxopts::value<string>(logFile)->default_value(string(home) + "/" + DEFAULT_LOG_DIR + "/midipatch.log"));

	auto result = options.parse(argc, argv);

	if (result["help"].count()) {
		std::cerr << options.help() << std::endl;
		exit(OPTIONS_ERROR);
	}
	current_channel = channel;
	midipatch::Logger::init(midipatch::L_DEBUG, logFile);
	Logger& logger = Logger::getInstance();
	PScriptsSingleton::init(sampleRate);
	pscript = PScriptsSingleton::getInstance();

	Websocket* websocket = nullptr;
	if (port > 0) {
		Websocket::init(port, patchFile);
		websocket = Websocket::getInstance();
		websocket->setMessageParsingErrorCallback([&](string type, string msg) {
			logger.error("Message parsing error", type + "(" + msg + ")");
		});
	}

	pscript->setErrorHandler([&](int status, const char* msg) {
		switch (status) {
			case LUA_ERRSYNTAX:
			logger.syntax_error("Syntax error", msg);
			break;
			case LUA_ERRRUN:
			logger.error("Runtime error", msg);
			break;
			case LUA_ERRMEM:
			logger.error("Lua memory allocation error", msg);
			break;
			case LUA_ERRERR:
			logger.error("Error running error", msg);
			break;
#if LUA_VERSION_NUM >= 502
			case LUA_ERRGCMM:
			logger.error("GC error", msg);
			break;
#endif
			default:
#ifdef __MIDIPATCH_DEBUG
			logger.error("Lua unknown error", msg);
#endif
			break;
		}
	});

	std::vector<RtMidiIn*> midiIn(midiIndex.size(), nullptr);
	std::vector<miby_t*> miby_State(midiIndex.size());


	while (true) {
		RtAudio dac;
		RtAudio::StreamParameters rtParams;
		rtParams.deviceId = audioIndex;
		rtParams.nChannels = audio_channels;

		logger.info("Waiting for clients");
		while (websocket && !websocket->hasClients()) {
			sleep(1);
		}
		try {
			auto initSuccess = pscript->init(patchFile, numVoices);
			if (!initSuccess.first) {
				logger.error(initSuccess.second);
			}

			setup_websocket_callbacks(pscript, &midiIndex, &midiIn, &dac, rtParams, patchFile, sampleRate, bufferFrames);

			if(!pscript->getPolySynth()->getVoices().empty()) {
				if (websocket) {
					websocket->reset();
					websocket->sendConfig();
					websocket->sendControlList();
					websocket->sendSessionList();
				}
				for (size_t i = 0; i < midiIndex.size(); ++i) {
					try {
						const int& mi = midiIndex[i];
						miby_State[i] = new miby_t;

						logger.info("Setting up miby parser "  + std::to_string(i));
						miby_init(miby_State[i], miby_State[i]); // self as user-data

						logger.info("Initialize MIDI", "Port: " + std::to_string(mi));
						midiIn[i] = new RtMidiIn();
						midiIn[i]->openPort(mi);
						midiIn[i]->setCallback(&mainMIDICallback, miby_State[i]);
					} catch (std::exception& e) {
						logger.warn("Unable to open midi port");
					}
				}

				try {
					logger.info("Initialize Audio",
							"Port: " + std::to_string(rtParams.deviceId) + " channels: " + std::to_string(rtParams.nChannels)
									+ " rate: " + std::to_string(sampleRate) + " frames: " + std::to_string(bufferFrames));

					dac.openStream(&rtParams, NULL, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &renderCallback, NULL, NULL);
					dac.startStream();
				} catch (std::exception& e) {
					logger.warn("Unable to open audio port");
				}

				while (!websocket || !websocket->isRestartRequested()) {
					sleep(1);
				}
				logger.info("Restart request");
				logger.info("Cleanup MIDI");

				for (size_t i = 0; i < midiIn.size(); ++i) {
					try {
						if (!midiIn[i])
							continue;
						midiIn[i]->cancelCallback();
						midiIn[i]->closePort();
						delete (midiIn[i]);
					} catch (std::exception& e) {
						logger.warn("Can't clean up MIDI port");
					}
				}
				logger.info("Cleanup Audio");
				try {
					dac.abortStream();
					dac.closeStream();
				} catch (std::exception& e) {
					logger.warn("Can't clean up audio port");
				}

				pscript->destroy();

				while (dac.isStreamOpen()) {
					logger.warn("Waiting for audiostream to finish");
					sleep(1);
				}
				logger.info("Restarting");
			} else {
				if (websocket)
					websocket->reset();
				while (!websocket || !websocket->isRestartRequested()) {
					sleep(1);
				}
				logger.info("Restarting");
			}

		} catch (RtError& e) {
			logger.error("Exception", e.getMessage());
			exit(UNKNOWN_ERROR);
		} catch (std::exception& ex) {
			logger.error("Exception", ex.what());
			exit(UNKNOWN_ERROR);
		}
	}

	return 0;
}

