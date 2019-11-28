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
#include <iomanip>
#include <mutex>
#include "App.h"
#include "PolySynth.hpp"
#include "SessionObject.hpp"

std::string escape_json(const std::string &s);

namespace midipatch {

class Websocket {
	us_listen_socket_t* socket_;
	std::set<uWS::WebSocket<false, true>*> clients_;
	std::vector<std::string> buffers_;
	std::mutex mutex_;
	bool audioStreamEnabled_ = true;
	bool restart_ = false;
	std::function<void(string, float)> setControlCallback_;
	std::function<void(size_t, size_t)> noteOnCallback_;
	std::function<void(size_t)> noteOffCallback_;
	std::function<void()> clearAllNotesCallback_;
	std::function<string()> sendControlListCallback_;
	std::function<string()> sendConfigCallback_;
	std::function<string()> sendSessionListCallback_;
	std::function<string()> sendStatusReportCallback_;
	std::function<void(string,string)> messageParsingErrorCallback_;
	std::function<void(const patchscript::SessionObject&)> updateSessionCallback_;
	std::function<void(const patchscript::SessionObject&)> deleteSessionCallback_;

public:
	Websocket(size_t port, const string& bankFile);
	virtual ~Websocket();

	void setSendControlListCallback(std::function<string()> callback) {
		sendControlListCallback_ = callback;
	}

	void setSendSessionListCallback(std::function<string()> callback) {
		sendSessionListCallback_ = callback;
	}

	void setSendStatusReportCallback(std::function<string()> callback) {
		sendStatusReportCallback_ = callback;
	}

	void setUpdateSessionCallback(std::function<void(const patchscript::SessionObject&)> callback) {
		updateSessionCallback_ = callback;
	}

	void setDeleteSessionCallback(std::function<void(const patchscript::SessionObject&)> callback) {
		deleteSessionCallback_ = callback;
	}

	void setSetControlCallback(std::function<void(string, float)> callback) {
		setControlCallback_ = callback;
	}

	void setNoteOnCallback(std::function<void(size_t, size_t)> callback) {
		noteOnCallback_ = callback;
	}

	void setNoteOffCallback(std::function<void(size_t)> callback) {
		noteOffCallback_ = callback;
	}

	void setClearAllNotesCallback(std::function<void()> callback) {
		clearAllNotesCallback_ = callback;
	}

	void setSendConfigCallback(std::function<string()> callback) {
		sendConfigCallback_ = callback;
	}

	void setMessageParsingErrorCallback(std::function<void(string,string)> callback) {
		messageParsingErrorCallback_ = callback;
	}

	void clear();
	void print(const uint8_t& col, const uint8_t& row, const std::string& s);
	void flush();
	void sendNoteOn(size_t note, size_t velocity);
	void sendNoteOff(size_t note);
	void sendAudio(const std::vector<float>& buf);
	void updateParameter(const string& name, const float& value);
	bool isAudioStreamEnabled() {
		return audioStreamEnabled_;
	}

	bool isRestartRequested() {
		return restart_;
	}

	void reset() {
		audioStreamEnabled_ = false;
		restart_ = false;
	}
	void sendConfig();
	void sendLogRecord(const string& title, const string& msg, int severity, bool highlight, bool lock = true);
	void sendControlList();
	void sendPatchList();
	size_t hasClients();
};

} /* namespace midipatch */

#endif /* SRC_WEBSOCKET_HPP_ */
