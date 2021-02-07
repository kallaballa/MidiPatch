#ifndef SRC_WEBSOCKET_CALLBACKS_HPP_
#define SRC_WEBSOCKET_CALLBACKS_HPP_

#include "PatchScript.hpp"
#include "RtMidi.h"
#include "RtAudio.h"
#include "websocket.hpp"

namespace midipatch {

void setup_websocket_callbacks(patchscript::PatchScript* pscript, std::vector<int>* midiIndex, std::vector<RtMidiIn*>* midiIn, RtAudio* dac, RtAudio::StreamParameters params, string patchFile, unsigned int sampleRate, unsigned int bufferFrames);

} /* namespace midipatch */

#endif /* SRC_WEBSOCKET_CALLBACKS_HPP_ */
