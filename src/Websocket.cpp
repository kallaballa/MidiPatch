#include "Websocket.hpp"
#include "defines.hpp"
#include <thread>
#include <mutex>
#include <iomanip>
#include "nlohmann/json.hpp"

// for convenience
using json = nlohmann::json;

namespace farts {

std::string escape_json(const std::string &s) {
    std::ostringstream o;
    for (auto c = s.cbegin(); c != s.cend(); c++) {
        switch (*c) {
        case '"': o << "\\\""; break;
        case '\\': o << "\\\\"; break;
        case '\b': o << "\\b"; break;
        case '\f': o << "\\f"; break;
        case '\n': o << "\\n"; break;
        case '\r': o << "\\r"; break;
        case '\t': o << "\\t"; break;
        default:
            if ('\x00' <= *c && *c <= '\x1f') {
                o << "\\u"
                  << std::hex << std::setw(4) << std::setfill('0') << (int)*c;
            } else {
                o << *c;
            }
        }
    }
    return o.str();
}

Websocket::Websocket(PolySynth& synth, size_t port) : buffers_(4){
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
      },
      .message = [&](auto *ws, std::string_view message, uWS::OpCode opCode) {
      	json msg = json::parse(std::string(message));
      	std::string type = msg["type"];
      	if(type == "list-controls") {
        	std::ostringstream ss;
        	ss << "{ \"type\": \"control-list\", \"data\": [ ";

      		auto params = synth.getVoices()[0].synth.getParameters();
      		std::vector<string> parents;
      		std::map<string, std::vector<std::pair<string, float>>> hierachie;
      		for(size_t i = 0; i < params.size(); ++i) {
      		      const string& name = params[i].getName();
      		      if(name.empty() || name.at(0) == '_')
      		      	continue;
      		      string parent;
      		      string child;
      		      auto pos = name.find(".");
      		      if(pos != string::npos && pos < name.size() - 1) {
      		      	parent = name.substr(0, pos);
      		      	child = name.substr(pos + 1);
      		      } else if(!name.empty()){
      		      	parent = "Global";
      		      	child = name;
      		      }
      		      if(std::find(parents.begin(), parents.end(), parent) == parents.end()) {
      		      	parents.push_back(parent);
      		      }
      		      hierachie[parent].push_back({child, params[i].getValue()});
      		}

      		size_t i = 0;
      		for(const auto& parent : parents) {
      			const string& module = parent;
      			const std::vector<std::pair<string,float>>& children = hierachie[parent];
      			ss << "{ \"name\": \"" << escape_json(module) << "\", \"controls\": [";
      			size_t j = 0;
      			for(const auto& child : children) {
      				ss << "{ \"name\" :\"" << escape_json(child.first) << "\", \"value\": \"" << child.second << "\" }";
      				if(j < children.size() - 1)
      					ss << ',';

      				++j;
      			}
      			ss << "]}";
     				if(i < hierachie.size() - 1)
        					ss << ',';
      			++i;
      		}
      		ss << "]}";
      		ws->send(ss.str(), uWS::TEXT);
      	} else if(type == "set-control") {
      		string name = msg["name"];
      		float value = msg["value"].get<float>();
      		for(auto& voice : synth.getVoices()) {
      			voice.synth.setParameter(name, value);
      		}
      	}
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

void Websocket::updateParameter(const string& name, const float& value) {
	std::scoped_lock lock(mutex_);
	std::ostringstream ss;
  string parent;
  string child;
  auto pos = name.find(".");
  if(pos != string::npos && pos < name.size() - 1) {
  	parent = name.substr(0, pos);
  	child = name.substr(pos + 1);
  } else if(!name.empty()){
  	parent = "Global";
  	child = name;
  }
	ss << "{ \"type\": \"update-control\", \"data\": { \"parent\": \"" << escape_json(parent) <<  "\", \"child\": \"" << escape_json(child) <<  "\", \"value\": \"" << value << "\"} }";
	for(auto& client: clients) {
		client->send(ss.str(), uWS::TEXT);
	}
}
void Websocket::flush() {
	std::scoped_lock lock(mutex_);
	std::ostringstream ss;
	ss << "{ \"type\": \"display\", \"data\": \"" << escape_json(buffers_[0] + buffers_[1] + buffers_[2] + buffers_[3]) <<  "\"}";
	for(auto& client: clients) {
		client->send(ss.str(), uWS::TEXT);
	}
}
} /* namespace farts */
