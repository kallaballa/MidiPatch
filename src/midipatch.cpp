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

#include "tonic_lua.hpp"
#include "RtAudio.h"
#include "RtMidi.h"
#include "RtError.h"
#include "PolySynth.hpp"
#include "LCD.hpp"
#include "Websocket.hpp"
#include "UDP.hpp"

using namespace Tonic;

const unsigned int nChannels = 2;

Synth* synth;
PolySynth* poly;

uint8_t current_program = 127;
size_t controlNumberOffset = 0;
LCD* lcd = nullptr;
midipatch::Websocket* websocket;
string save_file;

inline void print_red(const string& s, bool bold) {
	std::cerr << "\033["<< (bold ? "1;" : "") << "31m" << s << "\033[0m" << std::endl;
}

inline void ui_print(const uint8_t& col, const uint8_t& row, const string& s) {
	if (lcd)
		lcd->print(col, row, s);
	if (websocket)
		websocket->print(col, row, s);
}

inline void ui_clear() {
	if (lcd)
		lcd->clear();
	if (websocket)
		websocket->clear();
}

inline void ui_flush() {
	if (websocket)
		websocket->flush();
}

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
		poly->noteOff(b1);
	} else if (msgtype == 0x90) {
//		std::cout << "MIDI Note ON   C: " << chan << " N: " << b1 << " V: " << b2 << std::endl;
		poly->noteOn(b1, b2);
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
						return;
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

				ui_clear();
				ui_print(0, 0, parent);
				ui_print(0, 1, child + ": " + std::to_string(b2 / 127.0f));
				ui_flush();
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
	std::string appName = argv[0];
	int midiIndex = 0;
	int audioIndex = 0;
	unsigned int sampleRate = 48000;
	unsigned int bufferFrames = 512;
	string patchFile;
	string logFile;
	string ttyLCD;
	string saveFile;
	string bankFile;
	size_t numVoices;
	cxxopts::Options options(appName, "A scriptable, modular and real-time MIDI synthesizer");
	options.add_options()("h,help", "Print help messages")("m,midi", "The index of the midi input device to use.",
			cxxopts::value<int>(midiIndex)->default_value("0"))("a,audio", "The index of the audio output device to use.",
			cxxopts::value<int>(audioIndex)->default_value("0"))("r,rate", "The audio output sample rate.",
			cxxopts::value<unsigned int>(sampleRate)->default_value("44100"))("b,buffer", "Number of frames per buffer.",
			cxxopts::value<unsigned int>(bufferFrames)->default_value("32"))("l,lcd", "The tty file for the LCD display.",
			cxxopts::value<string>(ttyLCD))("o,offset", "The control number offset for parameter mapping",
			cxxopts::value<size_t>(controlNumberOffset)->default_value("52"))("v,voices", "The number of voices to run",
			cxxopts::value<size_t>(numVoices)->default_value("8"))("s,save",
			"The file where current patch settings are stored",
			cxxopts::value<string>(saveFile)->default_value("/tmp/midipatch.save"))("p,patchFile",
			"The lua patchFile to use for the voices", cxxopts::value<string>(patchFile))("f,logFile", "The file to log to",
			cxxopts::value<string>(logFile)->default_value("/tmp/midipatch.log"));

	auto result = options.parse(argc, argv);

	if (result["help"].count()) {
		std::cerr << options.help() << std::endl;
		exit(1);
	}

	if (!ttyLCD.empty()) {
		std::cerr << "Enabling LCD on " << ttyLCD << std::endl;
		lcd = new LCD(ttyLCD.c_str());
	}

	if (!saveFile.empty()) {
		save_file = saveFile;
	}
	std::cerr.flush();
	std::ofstream ofLog(logFile);
	std::cout.rdbuf(ofLog.rdbuf());
	std::cerr.rdbuf(ofLog.rdbuf());

	ui_clear();
	ui_print(0, 0, "Starting...");
	ui_flush();

	kaguya::State state;
	state.setErrorHandler([](int status, const char* msg){
    switch (status) {
    case LUA_ERRSYNTAX:
          print_red("Syntax error:", true);
          break;
    case LUA_ERRRUN:
      		print_red("Runtime error:", true);
      		break;
    case LUA_ERRMEM:
      print_red("Lua memory allocation error:", true);
      break;
    case LUA_ERRERR:
      print_red("Error running error:", true);
      break;
#if LUA_VERSION_NUM >= 502
    case LUA_ERRGCMM:
      		print_red("GC error:", true);
      		break;
#endif
    default:
    	print_red("Lua unknown error:", true);
    	break;
    }
		print_red(string("    ") + msg + "\n", false);
	});
	bindings0(state);
	bindings1(state);
	bindings2(state);

	// You don't necessarily have to do this - it will default to 44100 if not set.
	Tonic::setSampleRate(sampleRate);
	std::vector<Synth*> s(numVoices, nullptr);
	ui_clear();
	ui_print(0, 0, string("Loading patch..."));
	ui_flush();
	websocket = new midipatch::Websocket(8080, logFile, patchFile);
	RtMidiIn* midiIn;

	while (true) {
		RtAudio dac;
		RtAudio::StreamParameters rtParams;
		rtParams.deviceId = audioIndex;
		rtParams.nChannels = nChannels;

		synth = new Synth();
		poly = new PolySynth();
		midiIn = new RtMidiIn();
		try {
			for (size_t i = 0; i < numVoices; ++i) {
				if (s[i] != nullptr) {
					delete (s[i]);
				}
				s[i] = new Synth();
				state["synth"] = s[i];
				if(!state.dofile(patchFile)) {
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

				if (midiIn->getPortCount() == 0) {
					std::cerr << "No MIDI ports available!\n";
					cin.get();
					exit(0);
				}
				std::cerr << "Opening MIDI port: " << midiIndex << std::endl;
				midiIn->openPort(midiIndex);
				midiIn->setCallback(&midiCallback);

				std::cerr << "Opening audio port: " << rtParams.deviceId << " channels: " << rtParams.nChannels << " rate: "
						<< sampleRate << " frames: " << bufferFrames << std::endl;

				dac.openStream(&rtParams, NULL, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &renderCallback, NULL, NULL);
				dac.startStream();
				ui_clear();
				ui_print(0, 0, "Running");
				ui_flush();

				while (!websocket->isRestartRequested()) {
					sleep(1);
				}
				std::cerr << "Restarting" << std::endl;
				midiIn->cancelCallback();
				midiIn->closePort();
				dac.abortStream();
				dac.closeStream();
				delete (synth);
				delete (poly);
				delete (midiIn);
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
			print_red("Exception:",true);
			print_red(std::string("    ") + e.getMessage(),false);
			exit(2);
		} catch (std::exception& ex) {
			print_red("Exception:",true);
			print_red(std::string("    ") + ex.what(),false);
			exit(2);
		}
	}

	return 0;
}

