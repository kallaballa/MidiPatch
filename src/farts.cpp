#include <iostream>
#include <unistd.h>
#include <fstream>
#include <csignal>

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

using namespace Tonic;

const unsigned int nChannels = 2;

// Static smart pointer for our Synth
static Synth synth;
static PolySynth poly;

uint8_t current_program = 127;
size_t controlNumberOffset = 0;
LCD* lcd = nullptr;
static farts::Websocket* websocket;
string save_file;

inline void ui_print(const uint8_t& col, const uint8_t& row, const string& s) {
	if(lcd)
		lcd->print(col, row, s);
	if(websocket)
		websocket->print(col, row, s);
}

inline void ui_clear() {
	if(lcd)
		lcd->clear();
	if(websocket)
		websocket->clear();
}

inline void ui_flush() {
	if(websocket)
		websocket->flush();
}

int renderCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime,
		RtAudioStreamStatus status, void *userData) {
	synth.fillBufferOfFloats((float*) outputBuffer, nBufferFrames, nChannels);
	return 0;
}

void midiCallback(double deltatime, vector<unsigned char>* msg, void* userData) {
	int chan = (*msg)[0] & 0xf;
	int msgtype = (*msg)[0] & 0xf0;
	int b1 = (*msg)[1];
	int b2 = 0;
	if(msg->size() >= 2)
		b2 = (*msg)[2];

	if (msgtype == 0x80 || (msgtype == 0x90 && b2 == 0)) {
//		std::cout << "MIDI Note OFF  C: " << chan << " N: " << b1 << std::endl;
		poly.noteOff(b1);
	} else if (msgtype == 0x90) {
//		std::cout << "MIDI Note ON   C: " << chan << " N: " << b1 << " V: " << b2 << std::endl;
		poly.noteOn(b1, b2);
	} 	else if (msgtype == 0xB0) {
//		std::cout << "MIDI CC ON     C: " << chan << " N: " << b1 << " V: " << b2 << std::endl;
		std::vector<string> commonParams;

		//try to set a common parameter for all synths. NOTE: only works if all synthesizers have the same public parameters
		if(!poly.getVoices().empty()) {
			Synth s;
			std::vector<string> currentParams;
			bool first = true;

			//check if all parameters match between all synths
			for (auto& pv : poly.getVoices()) {
				currentParams.clear();
				s = pv.synth;
				for (auto& cp : s.getParameters()) {
					const string name = cp.getDisplayName();
					//Parameters with a name starting with '_' are private
					if (!name.empty() && name.at(0) != '_') {
						currentParams.push_back(name);
					}
				}
				if(first) {
					commonParams = currentParams;
					first = false;
				} else {
					if(currentParams != commonParams) {
						std::cerr << "Synth parameters differ, can't set parameters globally" << std::endl;
						return;
					}
				}
			}

			std::vector<string> publicParameters = currentParams;
			string parent;
			string child;
			for (auto& pv : poly.getVoices()) {
				s = pv.synth;

				if ((b1 - controlNumberOffset) < publicParameters.size()) {
					const string& name = publicParameters[b1 - controlNumberOffset];

					auto delim = name.find(".");

					if(delim != string::npos) {
						parent = name.substr(0, delim);
						child = name.substr(delim+1);
					} else {
						parent = "Global";
						child = name;
					}
					s.setParameter(name, (float) b2 / 127.0);
					if(websocket)
						websocket->updateParameter(name, (float) b2 / 127.0);
				}

					ui_clear();
					ui_print(0,0, parent);
					ui_print(0,1,child + ": " + std::to_string(b2 / 127.0f));
					ui_flush();
			}
		}
	} else if (msgtype == 0xC0) {
//		std::cout << "MIDI Program change  C: " << chan << " P: " << b1 << std::endl;
		current_program = b1;
	}

}

void save_parameters () {
	std::cerr << "saving parameters..." << std::endl;
	auto params = poly.getVoices()[0].synth.getParameters();
	ofstream ofs(save_file);
	for(auto& p : params) {
		ofs << p.getName() << (char)0 << std::to_string(p.getValue()) << (char)0;
	}
}

void load_parameters () {
	std::cerr << "loading parameters..." << std::endl;
	ifstream ifs(save_file);
	char buf0[1024];
	char buf1[1024];

	std::map<string, float> loadMap;
	while(ifs.getline(buf0, 1024, (char)0) && ifs.getline(buf1, 1024, (char)0)) {
		loadMap[string(buf0)] = std::stof(string(buf1));
		std::cerr << string(buf0) << ": " << string(buf1) << std::endl;
	}

	for(auto& v : poly.getVoices()) {
		auto params = v.synth.getParameters();

		for(auto& p : params) {
			auto it = loadMap.find(p.getName());
			if(it != loadMap.end()) {
				synth.addParameter(p.getName(), (*it).second);
			}
		}
	}
}

void signalHandler( int signum ) {
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
	string ttyLCD;
	string saveFile;
	size_t numVoices;
	cxxopts::Options options(appName, "A scriptable, modular and real-time MIDI synthesizer");
	options.add_options()
					("h,help", "Print help messages")
				  ("m,midi", "The index of the midi input device to use.", cxxopts::value<int>(midiIndex)->default_value("0"))
				  ("a,audio", "The index of the audio output device to use.", cxxopts::value<int>(audioIndex)->default_value("0"))
				  ("r,rate", "The audio output sample rate.", cxxopts::value<unsigned int>(sampleRate)->default_value("44100"))
				  ("b,buffer", "Number of frames per buffer.", cxxopts::value<unsigned int>(bufferFrames)->default_value("32"))
				  ("l,lcd", "The tty file for the LCD display.", cxxopts::value<string>(ttyLCD))
				  ("o,offset", "The control number offset for parameter mapping", cxxopts::value<size_t>(controlNumberOffset)->default_value("52"))
				  ("v,voices", "The number of voices to run", cxxopts::value<size_t>(numVoices)->default_value("8"))
					("s,save", "The file where current patch settings are stored", cxxopts::value<string>(saveFile)->default_value("/tmp/farts.save"))
					("p,patchFile", "The lua patchFile to use for the voices", cxxopts::value<string>(patchFile))
					;

	auto result = options.parse(argc, argv);

	if (result["help"].count()) {
		std::cerr << options.help() << std::endl;
		exit(1);
	}

	if(!ttyLCD.empty()) {
		std::cerr << "Enabling LCD on " << ttyLCD << std::endl;
		lcd = new LCD(ttyLCD.c_str());
	}

	if(!saveFile.empty()) {
		save_file = saveFile;
	}

	ui_clear();
	ui_print(0,0, "Starting...");
	ui_flush();
	kaguya::State state;
	bindings0(state);
	bindings1(state);
	bindings2(state);

  RtAudio dac;
   RtAudio::StreamParameters rtParams;
   rtParams.deviceId = audioIndex;
   rtParams.nChannels = nChannels;

	RtMidiIn *midiIn = new RtMidiIn();

	// You don't necessarily have to do this - it will default to 44100 if not set.
	Tonic::setSampleRate(sampleRate);
	std::vector<Synth> s(numVoices);
	ui_clear();
	ui_print(0,0, string("Loading patch..."));
	ui_flush();

	for (size_t i = 0; i < numVoices; ++i) {
		state["synth"] = &s[i];
		state.dofile(patchFile);
		poly.addVoice(s[i]);
	}

	load_parameters();
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGKILL, signalHandler);

	//add a slight ADSR to prevent clicking
	synth.setOutputGen(poly);
	websocket = new farts::Websocket(poly,8080);

	// open rtaudio stream and rtmidi port
	try {
		if (midiIn->getPortCount() == 0) {
			std::cerr << "No MIDI ports available!\n";
			cin.get();
			exit(0);
		}
		std::cerr << "Opening MIDI port: " << midiIndex << std::endl;
		midiIn->openPort(midiIndex);
		midiIn->setCallback(&midiCallback);

		std::cerr << "Opening audio port: " << rtParams.deviceId <<
				" channels: "	<< rtParams.nChannels <<
				" rate: " << sampleRate <<
				" frames: "	<< bufferFrames << std::endl;

    dac.openStream( &rtParams, NULL, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &renderCallback, NULL, NULL );
    dac.startStream();
  	ui_clear();
  	ui_print(0,0, "Running");
  	ui_flush();
	while(true) { sleep(10); };		
dac.stopStream();
	} catch (RtError& e) {
		std::cerr << '\n' << e.getMessage() << '\n' << std::endl;
		cin.get();
		exit(0);
	}
	return 0;
}

