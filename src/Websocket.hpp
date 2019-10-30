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

namespace farts {

class Websocket {
public:
	Websocket(size_t port);
	virtual ~Websocket();
	void clear();
	void print(const uint8_t& col, const uint8_t& row, const std::string& s);
	void send(const std::string& s);
};

} /* namespace farts */

#endif /* SRC_WEBSOCKET_HPP_ */
