#include "Tonic.h"
#include "kaguya/kaguya.hpp"
#include <iostream>
#include <boost/program_options.hpp>

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
size_t controlNumberOffset = 0;

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
	} 	else if (msgtype == 0xB0) {
		std::cout << "MIDI CC ON     C: " << chan << " N: " << b1 << " V: " << b2 << std::endl;
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

namespace po = boost::program_options;

int main(int argc, const char * argv[]) {
	std::string appName = argv[0];
	int midiIndex = 0;
	int audioIndex = 0;
	std::vector<string> patchFiles;

	namespace po = boost::program_options;
	po::options_description desc("Options");
	desc.add_options()
			("help,h", "Print help messages")
			("midi,m", po::value<int>(&midiIndex)->default_value(midiIndex), "The index of the midi input device to use.")
			("audio,a", po::value<int>(&audioIndex)->default_value(audioIndex), "The index of the audio output device to use.")
			("offset,o", po::value<size_t>(&controlNumberOffset)->default_value(controlNumberOffset), "The control number offset for parameter mapping");

	po::options_description hidden("Hidden options");
	hidden.add_options()("patch-files", po::value<vector<string>>(&patchFiles), "patch-files");

	po::options_description cmdline_options;
	cmdline_options.add(desc).add(hidden);

	po::positional_options_description positionalOptions;
	positionalOptions.add("patch-files", 32);

	po::options_description visible;
	visible.add(desc);
	po::variables_map vm;

	po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(positionalOptions).run(), vm); // throws on error

	if (vm.count("help")) {
		std::cerr << "Usage: farts [options] <patch file>..."
				<< std::endl;
		std::cerr << visible;
		exit(1);
	}

	po::notify(vm); // throws on error, so do after help in case
									// there are any problems



	kaguya::State state;
	bindings0(state);
	bindings1(state);
	bindings2(state);

// Configure RtAudio
	RtAudio dac;
	RtAudio::StreamParameters rtParams;
	rtParams.deviceId = audioIndex;
	rtParams.nChannels = nChannels;
	unsigned int sampleRate = 44100;
	unsigned int bufferFrames = 64; // 512 sample frames

	RtMidiIn *midiIn = new RtMidiIn();

	// You don't necessarily have to do this - it will default to 44100 if not set.
	Tonic::setSampleRate(sampleRate);
	std::vector<Synth> s(patchFiles.size());

	for (size_t i = 0; i < argc; ++i) {
		state["synth"] = &s[i];
		state.dofile(patchFiles[i]);
		poly.addVoice(s[i]);
	}
	synth.setOutputGen(poly);

	// open rtaudio stream and rtmidi port
	try {
		if (midiIn->getPortCount() == 0) {
			std::cout << "No MIDI ports available!\n";
			cin.get();
			exit(0);
		}
		midiIn->openPort(midiIndex);
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

