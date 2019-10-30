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

namespace farts {

class Websocket {
	std::set<uWS::WebSocket<false, true>*> clients;
	std::vector<std::string> buffers_;
	std::mutex mutex_;
public:
	Websocket(size_t port);
	virtual ~Websocket();
	void clear();
	void print(const uint8_t& col, const uint8_t& row, const std::string& s);
	void flush();
};

} /* namespace farts */

#endif /* SRC_WEBSOCKET_HPP_ */
