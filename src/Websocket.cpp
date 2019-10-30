#include "Websocket.hpp"
#include "defines.hpp"
#include <thread>
#include <mutex>

namespace farts {


Websocket::Websocket(size_t port) : buffers_(4){
  struct PerSocketData {

  };

  std::thread t([&]() {
  /* Very simple WebSocket echo server */
  uWS::App().ws<PerSocketData>("/*", {
      /* Settings */
      .compression = uWS::SHARED_COMPRESSOR,
      .maxPayloadLength = 16 * 1024,
      .idleTimeout = 10,
      .maxBackpressure = 1 * 1024 * 1204,
      /* Handlers */
      .open = [&](auto *ws, auto *req) {
      	std::scoped_lock lock(mutex_);
      	std::cerr << "WS client connected" << std::endl;
      	clients.insert(ws);
      	ws->subscribe("broadcast");
      },
      .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {

      },
      .drain = [](auto *ws) {
          /* Check getBufferedAmount here */
      },
      .ping = [](auto *ws) {

      },
      .pong = [](auto *ws) {

      },
      .close = [&](auto *ws, int code, std::string_view message) {
      	std::scoped_lock lock(mutex_);
      	std::cerr << "WS client closed" << std::endl;
      	clients.erase(ws);
      }
  }).listen(8080, [=](auto *token) {
      if (token) {
          std::cout << "Listening on port " << 8080 << std::endl;
      }
  }).run();
  });

  t.detach();
}

Websocket::~Websocket() {
	// TODO Auto-generated destructor stub
}

void Websocket::clear() {
	buffers_[0] = "\n";
	buffers_[1] = "\n";
	buffers_[2] = "\n";
	buffers_[3] = "\n";
}

void Websocket::print(const uint8_t& col, const uint8_t& row, const std::string& s) {
	std::ostringstream ss;

	std::string spaces(UI_WIDTH, ' ');
	ss << spaces.substr(0, col);
	if(s.size() + col > UI_WIDTH) {
		ss << s.substr(0, UI_WIDTH - col);
	} else if(s.size() + col <= UI_WIDTH) {
		ss << s;
	}
	ss << '\n';

	buffers_[row] = ss.str();
}

void Websocket::flush() {
	std::scoped_lock lock(mutex_);
	if(!clients.empty()) {
		std::ostringstream ss;
		ss << buffers_[0] << buffers_[1] << buffers_[2] << buffers_[3];
		(*clients.begin())->publish("broadcast", ss.str(), uWS::TEXT);
	}
}
} /* namespace farts */
