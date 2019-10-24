#include "Tonic.h"
#include "kaguya/kaguya.hpp"
extern "C" {
#include "lua.h"
}
#include <iostream>
#include "RtAudio.h"
#include "tonic_lua.hpp"
#include "RtAudio.h"
#include "RtMidi.h"
#include "RtError.h"
#include "PolySynth.hpp"

using namespace Tonic;

const unsigned int nChannels = 2;

// Static smart pointer for our Synth
static Synth synth;
static PolySynth poly;

uint8_t current_program = 127;

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
		std::cout << "MIDI Note OFF  C: " << chan << " N: " << b1 << std::endl;
		poly.noteOff(b1);
	} else if (msgtype == 0x90) {
		std::cout << "MIDI Note ON   C: " << chan << " N: " << b1 << " V: " << b2 << std::endl;
		poly.noteOn(b1, b2);
	}
	else if (msgtype == 0xB0) {
//		std::cout << "MIDI CC ON   C: " << chan << " N: " << b1 << " V: " << b2 << std::endl;
		std::vector<string> commonParams;

		//try to set a common parameter for all synths. NOTE: only works if all synthesizers have the same public parameters
		if(current_program == 127 && !poly.getVoices().empty()) {
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

			for (auto& pv : poly.getVoices()) {
				s = pv.synth;

				if ((b1 - 52) < publicParameters.size()) {
					const string& name = publicParameters[b1 - 52];
					std::cout << name << ": " << (float) b2 / 127.0 << std::endl;
					s.setParameter(name, (float) b2 / 127.0);
				}
			}
		} else if(current_program < poly.getVoices().size()) {
			Synth s = poly.getVoices()[current_program].synth;
			std::vector<string> publicParameters;
			for(auto& cp : s.getParameters()) {
				const string name = cp.getDisplayName();
				if(!name.empty() && name.at(0) != '_') {
					publicParameters.push_back(name);
				}
			}
			if((b1 - 52) < publicParameters.size()) {
				const string& name = publicParameters[b1 - 52];
				std::cout << name << ": " << (float)b2/127.0 << std::endl;
				s.setParameter(name, (float)b2/127.0);
			}
		}
	} else if (msgtype == 0xC0) {
		std::cout << "MIDI Program change  C: " << chan << " P: " << b1 << std::endl;
		current_program = b1;
	}
}

int main(int argc, const char * argv[]) {
	kaguya::State state;
	bindings(state);

//    state["synth"] = &synth;
//    state.dofile("synth.lua");

// Configure RtAudio
	RtAudio dac;
	RtAudio::StreamParameters rtParams;
	rtParams.deviceId = dac.getDefaultOutputDevice();
	rtParams.nChannels = nChannels;
	unsigned int sampleRate = 44100;
	unsigned int bufferFrames = 512; // 512 sample frames

	RtMidiIn *midiIn = new RtMidiIn();

	// You don't necessarily have to do this - it will default to 44100 if not set.
	Tonic::setSampleRate(sampleRate);
	Synth s[argc - 1];
	size_t ccoffset = 52;

	for (size_t i = 2; i < argc; ++i) {
		s[i - 1] = Synth();
		state["synth"] = &s[i - 1];
		state.dofile(argv[i]);
		poly.addVoice(s[i - 1]);
	}
	synth.setOutputGen(poly);

	// open rtaudio stream and rtmidi port
	try {
		if (midiIn->getPortCount() == 0) {
			std::cout << "No MIDI ports available!\n";
			cin.get();
			exit(0);
		}
		midiIn->openPort(atoi(argv[1]));
		midiIn->setCallback(&midiCallback);

		dac.openStream(&rtParams, NULL, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &renderCallback, NULL, NULL);
		dac.startStream();

		printf("\n\nPress Enter to stop\n\n");
		cin.get();
		dac.stopStream();
	} catch (RtError& e) {
		std::cout << '\n' << e.getMessage() << '\n' << std::endl;
		cin.get();
		exit(0);
	}
	return 0;
}

