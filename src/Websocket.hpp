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
#include <memory>
#include <mutex>
#include "App.h"
#include "PolySynth.hpp"

namespace farts {

class Websocket {
	PolySynth& poly_;
	std::shared_ptr<uWS::App> app_;
	us_listen_socket_t* socket_;
	std::set<uWS::WebSocket<false, true>*> clients_;
	std::vector<std::string> buffers_;
	std::mutex mutex_;
	bool audioStreamEnabled_ = false;
	bool restart_ = false;

public:
	Websocket(PolySynth& poly, size_t port, const string& logFile, const string& bankFile);
	virtual ~Websocket();
	void sendControlList();
	void clear();
	void print(const uint8_t& col, const uint8_t& row, const std::string& s);
	void flush();
	void sendAudio(int16_t* audioBuffer, size_t len);
	void updateParameter(const string& name, const float& value);
	bool isAudioStreamEnabled() {
		return audioStreamEnabled_;
	}

	bool isRestartRequested() {
			return restart_;
		}
};

} /* namespace farts */

#endif /* SRC_WEBSOCKET_HPP_ */
