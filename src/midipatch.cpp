#include <iostream>
#include <unistd.h>
#include <fstream>
#include <csignal>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mutex>

#include "Tonic.h"
#include "kaguya/kaguya.hpp"
#include "cxxopts.hpp"
#include "RtAudio.h"

#include "logger.hpp"
#include "tonic_lua.hpp"
#include "RtAudio.h"
#include "RtMidi.h"
#include "RtError.h"
#include "PolySynth.hpp"
#include "Websocket.hpp"

using namespace Tonic;

const unsigned int nChannels = 2;

Synth* synth;
PolySynth* poly;

uint8_t current_program = 127;
size_t controlNumberOffset = 0;
midipatch::Websocket* websocket;
string save_file;

int renderCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime,
		RtAudioStreamStatus status, void *userData) {
	if (websocket->isRestartRequested())
		return 0;
	synth->fillBufferOfFloats((float*) outputBuffer, nBufferFrames, nChannels);

//	if (websocket->isAudioStreamEnabled() && udp) {
//		size_t lenBuf = nBufferFrames * nChannels;
//		std::vector<int16_t> samples(lenBuf, 0);
//
//		for (size_t i = 0; i < lenBuf; ++i) {
//			samples[i] = ((((float*) outputBuffer)[i] * 2.0) - 1.0) * std::numeric_limits<int16_t>::max();
//		}
//		udp->send(samples);
//	}
//		if(websocket)
//			websocket->sendAudio(samples.data(), lenBuf);
	return 0;
}

void midiCallback(double deltatime, vector<unsigned char>* msg, void* userData) {
	if (websocket->isRestartRequested())
		return;

	int chan = (*msg)[0] & 0xf;
	int msgtype = (*msg)[0] & 0xf0;
	int b1 = (*msg)[1];
	int b2 = 0;
	if (msg->size() >= 2)
		b2 = (*msg)[2];

	if (msgtype == 0x80 || (msgtype == 0x90 && b2 == 0)) {
//		std::cout << "MIDI Note OFF  C: " << chan << " N: " << b1 << std::endl;
		websocket->sendNoteOff(b1);
		poly->noteOff(b1);
	} else if (msgtype == 0x90) {
//		std::cout << "MIDI Note ON   C: " << chan << " N: " << b1 << " V: " << b2 << std::endl;
		poly->noteOn(b1, b2);
		websocket->sendNoteOn(b1, b2);
	} else if (msgtype == 0xB0) {
//		std::cout << "MIDI CC ON     C: " << chan << " N: " << b1 << " V: " << b2 << std::endl;
		std::vector<string> commonParams;

		//try to set a common parameter for all synths. NOTE: only works if all synthesizers have the same public parameters
		if (!poly->getVoices().empty()) {
			Synth s;
			std::vector<string> currentParams;
			bool first = true;

			//check if all parameters match between all synths
			for (auto& pv : poly->getVoices()) {
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
						std::cerr << "Synth parameters differ, can't set parameters globally" << std::endl;
						break;
					}
				}
			}

			std::vector<string> publicParameters = currentParams;
			string parent;
			string child;
			for (auto& pv : poly->getVoices()) {
				s = pv.synth;

				if ((b1 - controlNumberOffset) < publicParameters.size()) {
					const string& name = publicParameters[b1 - controlNumberOffset];

					auto delim = name.find(".");

					if (delim != string::npos) {
						parent = name.substr(0, delim);
						child = name.substr(delim + 1);
					} else {
						parent = "Global";
						child = name;
					}
					s.setParameter(name, (float) b2 / 127.0);
					if (websocket)
						websocket->updateParameter(name, (float) b2 / 127.0);
				}
			}
		}
	} else if (msgtype == 0xC0) {
//		std::cout << "MIDI Program change  C: " << chan << " P: " << b1 << std::endl;
		current_program = b1;
	}

}

void save_parameters() {
	auto params = poly->getVoices()[0].synth.getParameters();
	ofstream ofs(save_file);
	for (auto& p : params) {
		ofs << p.getName() << (char) 0 << std::to_string(p.getValue()) << (char) 0;
	}
}

void load_parameters() {
	ifstream ifs(save_file);
	char buf0[1024];
	char buf1[1024];

	std::map<string, float> loadMap;
	while (ifs.getline(buf0, 1024, (char) 0) && ifs.getline(buf1, 1024, (char) 0)) {
		loadMap[string(buf0)] = std::stof(string(buf1));
	}

	for (auto& v : poly->getVoices()) {
		auto params = v.synth.getParameters();

		for (auto& p : params) {
			auto it = loadMap.find(p.getName());
			if (it != loadMap.end()) {
				v.synth.setParameter(p.getName(), (*it).second);
			}
		}
	}
}

void signalHandler(int signum) {
	save_parameters();
	exit(signum);
}

int main(int argc, char ** argv) {
	midipatch::Logger::init(midipatch::L_DEBUG);
	std::string appName = argv[0];
	std::vector<int> midiIndex;
	int audioIndex = 0;
	unsigned int sampleRate = 48000;
	unsigned int bufferFrames = 512;
	string patchFile;
	string logFile;
	string saveFile;
	string bankFile;
	size_t numVoices;
	cxxopts::Options options(appName, "A scriptable, modular and real-time MIDI synthesizer");
	options.add_options()("h,help", "Print help messages")
			("m,midi", "The indeces of the midi input ports to use.", cxxopts::value<std::vector<int>>(midiIndex)->default_value("0"))
			("a,audio", "The index of the audio output port to use.",	cxxopts::value<int>(audioIndex)->default_value("0"))
			("r,rate", "The audio output sample rate.",	cxxopts::value<unsigned int>(sampleRate)->default_value("44100"))
			("b,buffer", "Number of frames per buffer.", cxxopts::value<unsigned int>(bufferFrames)->default_value("32"))
			("o,offset", "The control number offset for parameter mapping",	cxxopts::value<size_t>(controlNumberOffset)->default_value("52"))
			("v,voices", "The number of voices to run",	cxxopts::value<size_t>(numVoices)->default_value("8"))
			("s,save", "The file where current patch settings are stored", cxxopts::value<string>(saveFile)->default_value("/tmp/midipatch.save"))
			("p,patchFile",	"The lua patchFile to use for the voices", cxxopts::value<string>(patchFile))
			("f,logFile", "The file to log to",	cxxopts::value<string>(logFile)->default_value("/tmp/midipatch.log"));

	auto result = options.parse(argc, argv);

	if (result["help"].count()) {
		std::cerr << options.help() << std::endl;
		exit(4);
	}

	if (!saveFile.empty()) {
		save_file = saveFile;
	}
	std::cerr.flush();
	std::ofstream ofLog(logFile);
	std::cout.rdbuf(ofLog.rdbuf());
	std::cerr.rdbuf(ofLog.rdbuf());

	kaguya::State state;
	state.setErrorHandler([](int status, const char* msg) {
		switch (status) {
			case LUA_ERRSYNTAX:
				LOG_ERR_MSG("Syntax error", msg);
			break;
			case LUA_ERRRUN:
				LOG_ERR_MSG("Runtime error", msg);
			break;
			case LUA_ERRMEM:
				LOG_ERR_MSG("Lua memory allocation error", msg);
			break;
			case LUA_ERRERR:
				LOG_ERR_MSG("Error running error", msg);
			break;
#if LUA_VERSION_NUM >= 502
			case LUA_ERRGCMM:
				LOG_ERR_MSG("GC error", msg);
			break;
#endif
			default:
#ifdef __MIDIPATCH_DEBUG
				LOG_DEBUG_MSG("Lua unknown error:", msg);
#endif
			break;
		}
	});
	bindings0(state);
	bindings1(state);
	bindings2(state);

	// You don't necessarily have to do this - it will default to 44100 if not set.
	Tonic::setSampleRate(sampleRate);
	std::vector<Synth*> s(numVoices, nullptr);
	websocket = new midipatch::Websocket(8080, logFile, patchFile);
	std::vector<RtMidiIn*> midiIn(midiIndex.size(),nullptr);

	while (true) {
		RtAudio dac;
		RtAudio::StreamParameters rtParams;
		rtParams.deviceId = audioIndex;
		rtParams.nChannels = nChannels;

		synth = new Synth();
		poly = new PolySynth();
		try {
			for (size_t i = 0; i < numVoices; ++i) {
				if (s[i] != nullptr) {
					delete (s[i]);
				}
				s[i] = new Synth();
				state["synth"] = s[i];
				if (!state.dofile(patchFile)) {
					break;
				}
				poly->addVoice(*s[i]);
			}
			websocket->setSendConfigCallback([&]() {
				std::ostringstream ss;
				ss << "{ \"type\": \"config\", \"data\": { ";
				ss << "\"patchFile\": \"" << patchFile << "\"";
				ss << "}}";
				return ss.str();
			});
			websocket->sendConfig();

			if (!poly->getVoices().empty()) {
				load_parameters();
				signal(SIGINT, signalHandler);
				signal(SIGTERM, signalHandler);
				signal(SIGKILL, signalHandler);

				//add a slight ADSR to prevent clicking
				synth->setOutputGen(*poly);
				// open rtaudio stream and rtmidi port

				websocket->setSetControlCallback([&](string n, float v) {
					for(auto& voice : poly->getVoices()) {
						voice.synth.setParameter(n, v);
					}
				});
				websocket->setNoteOnCallback([&](size_t note, size_t velo) {
					poly->noteOn(note, velo);
				});

				websocket->setNoteOffCallback([&](size_t note) {
					poly->noteOff(note);
				});

				websocket->setClearAllNotesCallback([&]() {
					poly->clearAllNotes();
				});
				websocket->setSendControlListCallback([&]() {
					std::ostringstream ss;
					ss << "{ \"type\": \"control-list\", \"data\": [ ";
					if(poly->getVoices().empty())
					return string("");
					auto params = poly->getVoices()[0].synth.getParameters();
					std::vector<string> parents;
					std::map<string, std::vector<std::pair<string, float>>> hierachie;
					for (size_t i = 0; i < params.size(); ++i) {
						const string& name = params[i].getName();
						if (name.empty() || name.at(0) == '_')
						continue;
						string parent;
						string child;
						auto pos = name.find(".");
						if (pos != string::npos && pos < name.size() - 1) {
							parent = name.substr(0, pos);
							child = name.substr(pos + 1);
						} else if (!name.empty()) {
							parent = "Global";
							child = name;
						}
						if (std::find(parents.begin(), parents.end(), parent) == parents.end()) {
							parents.push_back(parent);
						}
						hierachie[parent].push_back( {child, params[i].getValue()});
					}

					size_t i = 0;
					for (const auto& parent : parents) {
						const string& module = parent;
						const std::vector<std::pair<string, float>>& children = hierachie[parent];
						ss << "{ \"name\": \"" << escape_json(module) << "\", \"controls\": [";
						size_t j = 0;
						for (const auto& child : children) {
							ss << "{ \"name\" :\"" << escape_json(child.first) << "\", \"value\": \"" << child.second << "\" }";
							if (j < children.size() - 1)
							ss << ',';

							++j;
						}
						ss << "]}";
						if (i < hierachie.size() - 1)
						ss << ',';
						++i;
					}
					ss << "]}";
					return ss.str();
				});

				websocket->sendConfig();
				websocket->sendControlList();
				websocket->reset();

	  		for (size_t i = 0; i < midiIndex.size(); ++i) {
					try {
						const int& mi = midiIndex[i];
						std::cerr << "Opening MIDI port: " << mi << std::endl;
						midiIn[i] = new RtMidiIn();
						midiIn[i]->openPort(mi);
						midiIn[i]->setCallback(&midiCallback);
					} catch (std::exception& e) {
						LOG_ERR_STR("Midi port not found!");
					}
				}

				try {
					std::cerr << "Opening audio port: " << rtParams.deviceId << " channels: " << rtParams.nChannels << " rate: "
							<< sampleRate << " frames: " << bufferFrames << std::endl;
					dac.openStream(&rtParams, NULL, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &renderCallback, NULL, NULL);
					dac.startStream();
				} catch (std::exception& e) {
					LOG_ERR_STR("Unable to open audio port!");
				}

				while (!websocket->isRestartRequested()) {
					sleep(1);
				}
				std::cerr << "Restarting" << std::endl;
				for (size_t i = 0; i < midiIn.size(); ++i) {
					try {
						if(!midiIn[i])
							continue;
						midiIn[i]->cancelCallback();
						midiIn[i]->closePort();
						delete (midiIn[i]);
					} catch (std::exception& e) {
						LOG_ERR_STR("Error cleaning up MIDI port");
					}
				}
				try{
					dac.abortStream();
					dac.closeStream();
				} catch (std::exception& e) {
					LOG_ERR_STR("Error cleaning up Audio port");
				}
				delete (synth);
				delete (poly);
			} else {
				websocket->reset();
				while (!websocket->isRestartRequested()) {
					sleep(1);
				}
				std::cerr << "Restarting" << std::endl;
			}
			while (dac.isStreamOpen()) {
				std::cerr << "Stream still open" << std::endl;
				sleep(1);
			}
		} catch (RtError& e) {
			LOG_ERR_MSG("Exception", e.getMessage());
			exit(2);
		} catch (std::exception& ex) {
			LOG_ERR_MSG("Exception", ex.what());
			exit(2);
		}
	}

	return 0;
}

