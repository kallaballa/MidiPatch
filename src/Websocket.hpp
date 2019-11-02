/*
 * Websocket.hpp
 *
 *  Created on: Oct 30, 2019
 *      Author: elchaschab
 */

#ifndef SRC_WEBSOCKET_HPP_
#define SRC_WEBSOCKET_HPP_

#include <cstdint>
#include <cstddef>
#include <string>
#include <set>
#include <mutex>
#include "App.h"
#include "PolySynth.hpp"

namespace farts {

class Websocket {
	std::set<uWS::WebSocket<false, true>*> clients;
	std::vector<std::string> buffers_;
	std::mutex mutex_;
	bool audioStreamEnabled_ = false;
public:
	Websocket(Synth& synth, PolySynth& poly, size_t port, const unsigned int& channels, const unsigned int& bufferFrames);
	virtual ~Websocket();
	void clear();
	void print(const uint8_t& col, const uint8_t& row, const std::string& s);
	void flush();
	void sendAudio(int16_t* audioBuffer, size_t len);
	void updateParameter(const string& name, const float& value);
	bool isAudioStreamEnabled() {
		return audioStreamEnabled_;
	}
};

} /* namespace farts */

#endif /* SRC_WEBSOCKET_HPP_ */
