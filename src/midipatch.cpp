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
#include "webview.h"
#include "miby.h"

#include "logger.hpp"
#include "Websocket.hpp"
#include "PatchScript.hpp"

#define MIDIPATCH_VERSION "0.0.1"

using namespace Tonic;

enum ExitCodes {
	PATCH_SCRIPT_ERROR = 1, OPTIONS_ERROR = 2, UNKNOWN_ERROR = 3
};

const unsigned int nChannels = 2;
uint8_t current_program = 127;
size_t controlNumberOffset = 0;

std::mutex midiMutex;
patchscript::PatchScript* pscript = nullptr;
midipatch::Websocket* websocket = nullptr;

char specialChars[10] = {'-', '_', '.', '+', '!', '(', ')'};

bool isValidSessionName(const string& n) {
	if(n.empty() || n.size() > 255)
		return false;

	for(const auto& c : n) {
		if((c >= '\x30' && c <= '\x39') || (c >= '\x41' && c <= '\x5A') || (c >= '\x61' && c <= '\x7A')) {
			continue;
		} else {
			bool found = false;
			for(const auto& s : specialChars) {
				if(c == s) {
					found = true;
					break;
				}
			}
			if(found)
				continue;
		}
		return false;
	}

	return true;
}

void log_error(const string& title, const string& msg = "") {
	if(msg.empty())
		LOG_ERR_STR(title);
	else
		LOG_ERR_MSG(title, msg);
	if (websocket)
		websocket->sendLogRecord(title, msg, (int) midipatch::L_ERROR, false);
}

void log_warn(const string& title, const string& msg = "") {
	if(msg.empty())
		LOG_WARN_STR(title);
	else
		LOG_WARN_MSG(title, msg);
	if (websocket)
		websocket->sendLogRecord(title, msg, (int) midipatch::L_WARNING, false);
}
void log_debug(const string& title, const string& msg = "") {
	if(msg.empty())
		LOG_DEBUG_STR(title);
	else
		LOG_DEBUG_MSG(title, msg);
	if (websocket)
		websocket->sendLogRecord(title, msg, (int) midipatch::L_DEBUG, false);
}

void log_info(const string& title, const string& msg = "") {
	if(msg.empty())
		LOG_INFO_STR(title);
	else
		LOG_INFO_MSG(title, msg);

	if (websocket)
		websocket->sendLogRecord(title, msg, (int) midipatch::L_INFO, false);
}

void log_syntax_error(const string& title, const string& msg = "") {
	if(msg.empty())
		LOG_ERR_STR(title);
	else
		LOG_ERR_MSG(title, msg);

	if (websocket)
		websocket->sendLogRecord(title, msg, (int) midipatch::L_ERROR, true);
}

int renderCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime,
		RtAudioStreamStatus status, void *userData) {
	if (websocket && websocket->isRestartRequested()) {
		return 1;
	} else {
		pscript->fill((float*) outputBuffer, nBufferFrames, nChannels);
		size_t lenBuf = nBufferFrames * nChannels;
		std::vector<float> samples(lenBuf, 0);

		for (size_t i = 0; i < lenBuf; ++i) {
			samples[i] = ((float*) outputBuffer)[i];
		}
		if (websocket)
			websocket->sendAudio(samples);
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

	std::cout << "MIDIPATCH_Control_Change C:" << int(MIBY_CHAN(a_miby)) << " CC1: " 
				<< int(MIBY_ARG0(a_miby)) << " CC2: " << int(MIBY_ARG1(a_miby)) << std::endl;

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
					log_warn("Synth parameters differ, can't set parameters globally");
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
				if (websocket)
					websocket->updateParameter(name, (float) MIBY_ARG0(a_miby) / 127.0);
			}
		}

	}
}

void MIDIPATCH_Note_On(miby_this_t a_miby) {
	std::cout << "MIDIPATCH_Note_On C: " << (int)MIBY_CHAN(a_miby) 
				<< " N: " << (int)MIBY_ARG0(a_miby) << " V: " << (int)MIBY_ARG1(a_miby) << std::endl;

	pscript->getPolySynth()->noteOn((uint8_t)MIBY_ARG0(a_miby), (uint8_t)MIBY_ARG1(a_miby));

	if (websocket)
		websocket->sendNoteOn((int)MIBY_ARG0(a_miby), (int)MIBY_ARG1(a_miby));
}

void MIDIPATCH_Note_Off(miby_this_t a_miby) {
	std::cout << "MIDIPATCH_Note_Off C:" << (int)MIBY_CHAN(a_miby) << std::endl;

	pscript->getPolySynth()->noteOff((int)MIBY_ARG0(a_miby));

	if (websocket)
		websocket->sendNoteOff((int)MIBY_ARG0(a_miby));
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

		current_program = (int)MIBY_ARG0(a_miby);
}


//
// main entrypoint for MIDI messages
//
void mainMIDICallback(double deltatime, vector<unsigned char>* msg, void* userData) {
	std::unique_lock lock(midiMutex);
	if (websocket && websocket->isRestartRequested())
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
	unsigned int sampleRate = 48000;
	unsigned int bufferFrames = 512;
	size_t port = 8080;
	string patchFile;
	string logFile;
	string bankFile;
	size_t numVoices;
	const char* home = std::getenv(DEFAULT_ENV_VARIABLE);
	if(!home) {
		log_error(string("Can't find environment variable: ") + DEFAULT_ENV_VARIABLE);
		exit(PATCH_SCRIPT_ERROR);
	}

	cxxopts::Options options(appName, "A scriptable, modular and real-time MIDI synthesizer");
	options.add_options()("h,help", "Print help messages")
			("m,midi", "The indeces of the midi input ports to use.",
				cxxopts::value<std::vector<int>>(midiIndex)->default_value("0"))
			("a,audio",	"The index of the audio output port to use.",
					cxxopts::value<int>(audioIndex)->default_value("0"))
			("r,rate", "The audio output sample rate.",
					cxxopts::value<unsigned int>(sampleRate)->default_value("48000"))
			("b,buffer", "Number of frames per buffer.",
					cxxopts::value<unsigned int>(bufferFrames)->default_value("512"))
			("w,websocket", "The port number of the websocket server.",
					cxxopts::value<size_t>(port)->default_value("0"))
			("o,offset", "The control number offset for parameter mapping",
				cxxopts::value<size_t>(controlNumberOffset)->default_value("52"))
			("v,voices", "The number of voices to run",
					cxxopts::value<size_t>(numVoices)->default_value("8"))
			("p,patchFile",	"The lua patchFile to use for the voices",
					cxxopts::value<string>(patchFile)->default_value(string(home) + "/" + DEFAULT_DATA_DIR + "/midipatch.pat"))
			("f,logFile", "The file to log to",
					cxxopts::value<string>(logFile)->default_value(string(home) + "/" + DEFAULT_LOG_DIR + "/midipatch.log"));

	auto result = options.parse(argc, argv);

	if (result["help"].count()) {
		std::cerr << options.help() << std::endl;
		exit(OPTIONS_ERROR);
	}
	midipatch::Logger::init(midipatch::L_DEBUG, logFile);

	pscript = new patchscript::PatchScript(sampleRate);
	if (port > 0) {
		websocket = new midipatch::Websocket(port, patchFile);
		websocket->setMessageParsingErrorCallback([&](string type, string msg) {
			log_error("Message parsing error", type + "(" + msg + ")");
		});
	}

	pscript->setErrorHandler([](int status, const char* msg) {
		switch (status) {
			case LUA_ERRSYNTAX:
			log_syntax_error("Syntax error", msg);
			break;
			case LUA_ERRRUN:
			log_error("Runtime error", msg);
			break;
			case LUA_ERRMEM:
			log_error("Lua memory allocation error", msg);
			break;
			case LUA_ERRERR:
			log_error("Error running error", msg);
			break;
#if LUA_VERSION_NUM >= 502
			case LUA_ERRGCMM:
			log_error("GC error", msg);
			break;
#endif
			default:
#ifdef __MIDIPATCH_DEBUG
			log_error("Lua unknown error", msg);
#endif
			break;
		}
	});

	std::vector<RtMidiIn*> midiIn(midiIndex.size(), nullptr);
	std::vector<miby_t*> miby_State(midiIndex.size());

	std::thread webThread([](){
#ifdef WEBVIEW_GTK
    setenv("GTK_OVERLAY_SCROLLING", "0", 1);
#endif
		webview::webview w(true, nullptr);
		w.set_title("MidiPatch");
		w.set_size(960, 540, WEBVIEW_HINT_NONE);
		w.navigate("http://localhost:8080");
		w.run();
		exit(0);
	});

	webThread.detach();

	while (true) {
		RtAudio dac;
		RtAudio::StreamParameters rtParams;
		rtParams.deviceId = audioIndex;
		rtParams.nChannels = nChannels;

		log_info("Waiting for clients");
		while (websocket && !websocket->hasClients()) {
			sleep(1);
		}
		try {
			auto initSuccess = pscript->init(patchFile, numVoices);
			if (!initSuccess.first) {
				log_error(initSuccess.second);
			}

			if (websocket) {
				websocket->setSendConfigCallback([&]() {
					std::ostringstream ss;
					ss << "{ \"type\": \"config\", \"data\": { ";
					ss << "\"version\": \"" << MIDIPATCH_VERSION << "\",";
					ss << "\"patchFile\": \"" << patchFile << "\",";
					ss << "\"sampleRate\": " << sampleRate << ",";
					ss << "\"bufferFrames\": " << bufferFrames << ",";
					ss << "\"channels\": " << nChannels << "";
					ss << "}}";
					return ss.str();
				});
				websocket->sendConfig();

				websocket->setSendSessionListCallback([&]() {
					std::vector<patchscript::SessionObject> list;
					pscript->list(list);
					std::ostringstream ss;
					ss << "{ \"type\": \"patch-list\", \"list\": [";
					for(size_t i = 0; i < list.size(); ++i) {
						const auto& po = list[i];
						ss << "{ \"name\": \"" << escape_json(po.name_)
								<< "\", \"author\": \"" << escape_json(po.author_)
								<< "\", \"revision\": \"" << po.revision_
								<< "\", \"runtimeName\": \"" << escape_json(po.runtimeName_)
								<< "\", \"runtimeVersion\": \"" << escape_json(po.runtimeVersion_)
								<< "\", \"description\": \"" << escape_json(po.description_)
								<< "\", \"code\": \"" << escape_json(po.code_)
								<< "\", \"date\": " << po.date_
								<< ", \"layout\": \"" << escape_json(po.layout_)
								<< "\", \"parameters\": \"" << escape_json(po.parameters_)
								<< "\", \"keyboardBindings\": \"" << escape_json(po.keyboardBindings_)
								<< "\", \"midiBindings\": \"" << escape_json(po.midiBindings_)
								<< "\"}";
						if(i < list.size() - 1)
							ss << ",";
					}
					ss << "]}";
					return ss.str();
				});
				websocket->sendSessionList();
			}

			if(!pscript->getPolySynth()->getVoices().empty()) {
				if (websocket) {
					websocket->setSetControlCallback([&](string n, float v) {
						for(auto& voice : pscript->getPolySynth()->getVoices()) {
							voice.synth.setParameter(n, v);
						}
					});
					websocket->setNoteOnCallback([&](size_t note, size_t velo) {
						pscript->getPolySynth()->noteOn(note, velo);
					});

					websocket->setNoteOffCallback([&](size_t note) {
						pscript->getPolySynth()->noteOff(note);
					});

					websocket->setClearAllNotesCallback([&]() {
						pscript->getPolySynth()->clearAllNotes();
					});
					websocket->setSendControlListCallback([&]() {
						std::ostringstream ss;
						ss << "{ \"type\": \"control-list\", \"data\": [ ";
						if(pscript->getPolySynth()->getVoices().empty())
						return string("");
						auto params = pscript->getPolySynth()->getVoices()[0].synth.getParameters();
						std::vector<string> parents;
						struct CP {
							float value_;
							float min_;
							float max_;
							string displayName_;
						};
						std::map<string, std::vector<std::pair<string, CP>>> hierachie;
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
							hierachie[parent].push_back( {child, {params[i].getValue(), params[i].getMin(), params[i].getMax(), params[i].getDisplayName()}});
						}

						size_t i = 0;
						for (const auto& parent : parents) {
							const string& module = parent;
							const auto& children = hierachie[parent];
							ss << "{ \"name\": \"" << escape_json(module) << "\", \"controls\": [";
							size_t j = 0;
							for (const auto& child : children) {
								ss << "{ \"name\" :\"" << escape_json(child.first)
														<< "\", \"value\": \"" << child.second.value_
														<< "\", \"min\": \"" << child.second.min_
														<< "\", \"max\": \"" << child.second.max_
														<< "\", \"displayName\": \"" << child.second.displayName_ << "\" }";
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

					websocket->setSendStatusReportCallback([&]() {
						std::ostringstream ss;
						ss << "{ \"type\": \"status-report\", \"midiports\": [";
						for(size_t i = 0; i < midiIndex.size(); ++i) {
							const auto& index = midiIndex[i];
							const auto& midiPort = midiIn[i];
							ss << "{ \"index\": " << index
									<< ", \"open\": " << midiPort->isPortOpen()
									<< "}";
							if(i < midiIndex.size() - 1)
								ss << ",";
						}

						ss << "], \"audioport\": "
									<< "{ \"index\": " << audioIndex
									<< ", \"open\": " << dac.isStreamOpen()
									<< "}";
						ss << "}";
						return ss.str();
					});

					websocket->setUpdateSessionCallback([&](const patchscript::SessionObject& po) {
						if(!isValidSessionName(po.name_)) {
							log_error("Invalid session name", po.name_);
							return;
						}
						pscript->store(po);
						websocket->sendSessionList();
					});

					websocket->setDeleteSessionCallback([&](const patchscript::SessionObject& po) {
						pscript->remove(po);
						websocket->sendSessionList();
					});

					websocket->reset();
					websocket->sendConfig();
					websocket->sendControlList();
					websocket->sendSessionList();
				}
				for (size_t i = 0; i < midiIndex.size(); ++i) {
					try {
						const int& mi = midiIndex[i];
						miby_State[i] = new miby_t;

						log_info("Setting up miby parser "  + std::to_string(i));
						miby_init(miby_State[i], miby_State[i]); // self as user-data

						log_info("Initialize MIDI", "Port: " + std::to_string(mi));
						midiIn[i] = new RtMidiIn();
						midiIn[i]->openPort(mi);
						midiIn[i]->setCallback(&mainMIDICallback, miby_State[i]);
					} catch (std::exception& e) {
						log_warn("Unable to open midi port");
					}
				}

				try {
					log_info("Initialize Audio",
							"Port: " + std::to_string(rtParams.deviceId) + " channels: " + std::to_string(rtParams.nChannels)
									+ " rate: " + std::to_string(sampleRate) + " frames: " + std::to_string(bufferFrames));

					dac.openStream(&rtParams, NULL, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &renderCallback, NULL, NULL);
					dac.startStream();
				} catch (std::exception& e) {
					log_warn("Unable to open audio port");
				}

				while (!websocket || !websocket->isRestartRequested()) {
					sleep(1);
				}
				log_info("Restart request");
				log_info("Cleanup MIDI");

				for (size_t i = 0; i < midiIn.size(); ++i) {
					try {
						if (!midiIn[i])
							continue;
						midiIn[i]->cancelCallback();
						midiIn[i]->closePort();
						delete (midiIn[i]);
					} catch (std::exception& e) {
						log_warn("Can't clean up MIDI port");
					}
				}
				log_info("Cleanup Audio");
				try {
					dac.abortStream();
					dac.closeStream();
				} catch (std::exception& e) {
					log_warn("Can't clean up audio port");
				}

				pscript->destroy();

				while (dac.isStreamOpen()) {
					log_warn("Waiting for audiostream to finish");
					sleep(1);
				}
				log_info("Restarting");
			} else {
				if (websocket)
					websocket->reset();
				while (!websocket || !websocket->isRestartRequested()) {
					sleep(1);
				}
				log_info("Restarting");
			}

		} catch (RtError& e) {
			log_error("Exception", e.getMessage());
			exit(UNKNOWN_ERROR);
		} catch (std::exception& ex) {
			log_error("Exception", ex.what());
			exit(UNKNOWN_ERROR);
		}
	}

	return 0;
}

