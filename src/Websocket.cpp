#include "Websocket.hpp"
#include "App.h"
#include "defines.hpp"

namespace farts {

Websocket::Websocket(size_t port) {
  struct PerSocketData {

  };

  /* Very simple WebSocket echo server */
  uWS::App().ws<PerSocketData>("/*", {
      /* Settings */
      .compression = uWS::SHARED_COMPRESSOR,
      .maxPayloadLength = 16 * 1024,
      .idleTimeout = 10,
      .maxBackpressure = 1 * 1024 * 1204,
      /* Handlers */
      .open = [](auto *ws, auto *req) {

      },
      .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
          ws->send(message, opCode);
      },
      .drain = [](auto *ws) {
          /* Check getBufferedAmount here */
      },
      .ping = [](auto *ws) {

      },
      .pong = [](auto *ws) {

      },
      .close = [](auto *ws, int code, std::string_view message) {

      }
  }).listen(port, [=](auto *token) {
      if (token) {
          std::cout << "Listening on port " + port << std::endl;
      }
  }).run();
}

Websocket::~Websocket() {
	// TODO Auto-generated destructor stub
}

void Websocket::clear() {
	std::ostringstream ss;
	for(size_t i = 0; i < UI_HEIGHT; ++i) {
		for(size_t j = 0; j < UI_WIDTH; ++j) {
			ss << ' ';
		}
		ss << '\n';
	}

//	webSocket_.send(ss.str());
}

void Websocket::print(const uint8_t& col, const uint8_t& row, const std::string& s) {
	std::ostringstream ss;
	for(size_t i = 0; i < UI_HEIGHT; ++i) {
		if(i != col) {
			for(size_t j = 0; j < UI_WIDTH; ++j) {
				ss << ' ';
			}
			ss << '\n';
		} else {
			std::string spaces(UI_WIDTH, 20);
			ss << spaces.substr(0, col);
			if(s.size() + col > UI_WIDTH) {
				ss << s.substr(0, UI_WIDTH - col);
			} else if(s.size() + col < UI_WIDTH) {
				ss << s;
				ss << spaces.substr(0, UI_WIDTH - (s.size() + col));
			}
			ss << '\n';
		}
	}

//	webSocket_.send(ss.str());
}

void Websocket::send(const std::string& s) {
//	webSocket_.send(s);
}
} /* namespace farts */
