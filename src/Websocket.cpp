#include "Websocket.hpp"
#include "defines.hpp"
#include <thread>
#include <mutex>
#include <iomanip>
#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>

extern char _binary_index_min_pack_start;
extern char _binary_index_min_pack_end;

// for convenience
using json = nlohmann::json;

namespace farts {

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

string url_decode(string str) {
	string ret;
	char ch;
	int i, ii, len = str.length();

	for (i = 0; i < len; i++) {
		if (str[i] != '%') {
			if (str[i] == '+')
				ret += ' ';
			else
				ret += str[i];
		} else {
			sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
			ch = static_cast<char>(ii);
			ret += ch;
			i = i + 2;
		}
	}
	return ret;
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';

	}

	return ret;

}

std::string base64_decode(std::string const& encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_];
		in_++;
		if (i == 4) {
			for (i = 0; i < 4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = 0; j < i; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

		for (j = 0; (j < i - 1); j++)
			ret += char_array_3[j];
	}

	return ret;
}

std::string escape_json(const std::string &s) {
	std::ostringstream o;
	for (auto c = s.cbegin(); c != s.cend(); c++) {
		switch (*c) {
		case '"':
			o << "\\\"";
			break;
		case '\\':
			o << "\\\\";
			break;
		case '\b':
			o << "\\b";
			break;
		case '\f':
			o << "\\f";
			break;
		case '\n':
			o << "\\n";
			break;
		case '\r':
			o << "\\r";
			break;
		case '\t':
			o << "\\t";
			break;
		default:
			if ('\x00' <= *c && *c <= '\x1f') {
				o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int) *c;
			} else {
				o << *c;
			}
		}
	}
	return o.str();
}

Websocket::Websocket(PolySynth& poly, size_t port, const string& logFile, const string& patchFile) :
		poly_(poly), buffers_(4) {
	struct PerSocketData {

	};

	std::thread t([&]() {
		uWS::App().get("/index.html", [&](auto *res, auto *req) {
					res->writeHeader("Content-Type", "text/html");
					char* start = &_binary_index_min_pack_start;
					char* end = &_binary_index_min_pack_end;
					std::cerr << (size_t) start << " -> " << (size_t)end << std::endl;
					char* p = start;
					std::ostringstream ss;
					while(p != end) {
						ss << *p++;
					}
					res->end(ss.str());
				}).get("/cgi-bin/log.cgi", [&](auto *res, auto *req) {
					try {
						res->writeHeader("Content-Type", "text/plain");
						std::ifstream ifs(logFile);
						char buffer[1024];
						while(ifs.getline(buffer, 1024)) {
							res->write(buffer);
							res->write("\n");
						}
					} catch (std::exception& ex) {
						std::cerr << ex.what() << std::endl;
					}
					res->end("");
				}).get("/cgi-bin/loadPatch.cgi", [&](auto *res, auto *req) {

					try {
						res->writeHeader("Content-Type", "text/plain");
						std::ifstream ifs(patchFile);
						char buffer[1024];
						while(ifs.getline(buffer, 1024)) {
							res->write(buffer);
							res->write("\n");
						}
					} catch (std::exception& ex) {
						std::cerr << ex.what() << std::endl;
					}
					res->end("");
				}).post("/cgi-bin/savePatch.cgi", [&](auto *res, auto *req) {
					try {
						res->writeHeader("Content-Type", "text/plain");
						res->onData([=](std::string_view data, bool isLast) {
									std::ofstream ofs(patchFile);
									string str(data);
									auto delim = str.find('=');
									if(delim != string::npos && str.size() > delim) {
										str = str.substr(delim + 1);
									}
									ofs << url_decode(str);
									if(isLast)
									res->end("");
								});
					} catch (std::exception& ex) {
						std::cerr << ex.what() << std::endl;
					}
					res->end("");
				}).ws<PerSocketData>("/*", {
					/* Settings */
					.compression = uWS::SHARED_COMPRESSOR,
					.maxPayloadLength = 16 * 1024,
					.idleTimeout = 10,
					.maxBackpressure = 1 * 1024 * 1204,
					/* Handlers */
					.open = [&](auto *ws, auto *req) {
						std::scoped_lock lock(mutex_);
						std::cerr << "WS client connected" << std::endl;
						clients_.insert(ws);
						sendControlList();
					},
					.message = [&](auto *ws, std::string_view message, uWS::OpCode opCode) {
						json msg = json::parse(std::string(message));
						std::string type = msg["type"];
						if(type == "set-control") {
							string name = msg["name"];
							float value = msg["value"].get<float>();
							for(auto& voice : poly.getVoices()) {
								voice.synth.setParameter(name, value);
							}
						} else if(type == "note-on") {
							size_t note = msg["note"].get<size_t>();
							size_t velocity = msg["velocity"].get<size_t>();
							poly.noteOn(note,velocity);
						} else if(type == "note-off") {
							size_t note = msg["note"].get<size_t>();
							poly.noteOff(note);
						} else if(type == "audio-stream-enabled") {
							audioStreamEnabled_ = msg["data"].get<bool>();
						} else if(type == "restart") {
							us_listen_socket_close(0, socket_);
							std::cerr << "socket closed" << std::endl;
							for(auto& client : clients_) {
								client->close();
							}
							restart_ = true;
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
						clients_.erase(ws);
					}
				}).listen(8080, [=](auto *token) {
					if (token) {
						socket_ = token;
						std::cout << "Listening on port " << 8080 << std::endl;
					}
				}).run();
	});

	t.detach();
}

Websocket::~Websocket() {
}

void Websocket::sendControlList() {
	std::ostringstream ss;
	ss << "{ \"type\": \"control-list\", \"data\": [ ";

	auto params = poly_.getVoices()[0].synth.getParameters();
	std::vector<string> parents;
	std::map<string, std::vector<std::pair<string, float>>> hierachie;
	for (size_t i = 0; i < params.size(); ++i) {
		const string& name = params[i].getName();
		if (name.empty() || name.at(0) == '_')
			continue;
		string parent;
		string child;
		auto pos = name.find(".");
		if (pos != string::npos && pos < name.size() - 1) {
			parent = name.substr(0, pos);
			child = name.substr(pos + 1);
		} else if (!name.empty()) {
			parent = "Global";
			child = name;
		}
		if (std::find(parents.begin(), parents.end(), parent) == parents.end()) {
			parents.push_back(parent);
		}
		hierachie[parent].push_back( { child, params[i].getValue() });
	}

	size_t i = 0;
	for (const auto& parent : parents) {
		const string& module = parent;
		const std::vector<std::pair<string, float>>& children = hierachie[parent];
		ss << "{ \"name\": \"" << escape_json(module) << "\", \"controls\": [";
		size_t j = 0;
		for (const auto& child : children) {
			ss << "{ \"name\" :\"" << escape_json(child.first) << "\", \"value\": \"" << child.second << "\" }";
			if (j < children.size() - 1)
				ss << ',';

			++j;
		}
		ss << "]}";
		if (i < hierachie.size() - 1)
			ss << ',';
		++i;
	}
	ss << "]}";
	for (auto& client : clients_) {
		client->send(ss.str(), uWS::TEXT);
	}
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
	if (s.size() + col > UI_WIDTH) {
		ss << s.substr(0, UI_WIDTH - col);
	} else if (s.size() + col <= UI_WIDTH) {
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
	if (pos != string::npos && pos < name.size() - 1) {
		parent = name.substr(0, pos);
		child = name.substr(pos + 1);
	} else if (!name.empty()) {
		parent = "Global";
		child = name;
	}
	ss << "{ \"type\": \"update-control\", \"data\": { \"parent\": \"" << escape_json(parent) << "\", \"child\": \""
			<< escape_json(child) << "\", \"value\": \"" << value << "\"} }";
	for (auto& client : clients_) {
		client->send(ss.str(), uWS::TEXT);
	}
}

void Websocket::sendAudio(int16_t* audioBuffer, size_t len) {
	std::scoped_lock lock(mutex_);
	string data = base64_encode((const unsigned char*) audioBuffer, sizeof(int16_t) * len);
	std::ostringstream ss;
	ss << "{ \"type\": \"audio-buffer\", \"data\": \"" << data << "\"}";
	for (auto& client : clients_) {
		client->send(ss.str(), uWS::TEXT);
	}
}

void Websocket::flush() {
	std::scoped_lock lock(mutex_);
	std::ostringstream ss;
	ss << "{ \"type\": \"display\", \"data\": \"" << escape_json(buffers_[0] + buffers_[1] + buffers_[2] + buffers_[3])
			<< "\"}";
	for (auto& client : clients_) {
		client->send(ss.str(), uWS::TEXT);
	}
}
} /* namespace farts */
