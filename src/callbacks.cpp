#include "callbacks.hpp"
#include "logger.hpp"
#include "version.hpp"

namespace midipatch {

char specialChars[10] = { '-', '_', '.', '+', '!', '(', ')' };

bool isValidSessionName(const string& n) {
	if (n.empty() || n.size() > 255)
		return false;

	for (const auto& c : n) {
		if ((c >= '\x30' && c <= '\x39') || (c >= '\x41' && c <= '\x5A') || (c >= '\x61' && c <= '\x7A')) {
			continue;
		} else {
			bool found = false;
			for (const auto& s : specialChars) {
				if (c == s) {
					found = true;
					break;
				}
			}
			if (found)
				continue;
		}
		return false;
	}

	return true;
}

void setup_websocket_callbacks(patchscript::PatchScript* pscript, std::vector<int>* midiIndex, std::vector<RtMidiIn*>* midiIn, RtAudio* dac, RtAudio::StreamParameters params, string patchFile, unsigned int sampleRate, unsigned int bufferFrames) {
	Websocket* websocket = Websocket::getInstance();
	if (websocket) {
		websocket->setSendConfigCallback([=]() {
			std::ostringstream ss;
			ss << "{ \"type\": \"config\", \"data\": { ";
			ss << "\"version\": \"" << MIDIPATCH_VERSION << "\",";
			ss << "\"patchFile\": \"" << patchFile << "\",";
			ss << "\"sampleRate\": " << sampleRate << ",";
			ss << "\"bufferFrames\": " << bufferFrames << ",";
			ss << "\"channels\": " << params.nChannels << "";
			ss << "}}";
			return ss.str();
		});
		websocket->sendConfig();

		websocket->setSendSessionListCallback([=]() {
			std::vector<patchscript::SessionObject> list;
			pscript->list(list);
			std::ostringstream ss;
			ss << "{ \"type\": \"patch-list\", \"list\": [";
			for(size_t i = 0; i < list.size(); ++i) {
				const auto& po = list[i];
				ss << "{ \"name\": \"" << escape_json(po.name_)
				<< "\", \"author\": \"" << escape_json(po.author_)
				<< "\", \"revision\": \"" << po.revision_
				<< "\", \"runtimeName\": \"" << escape_json(po.runtimeName_)
				<< "\", \"runtimeVersion\": \"" << escape_json(po.runtimeVersion_)
				<< "\", \"description\": \"" << escape_json(po.description_)
				<< "\", \"code\": \"" << escape_json(po.code_)
				<< "\", \"date\": " << po.date_
				<< ", \"layout\": \"" << escape_json(po.layout_)
				<< "\", \"parameters\": \"" << escape_json(po.parameters_)
				<< "\", \"keyboardBindings\": \"" << escape_json(po.keyboardBindings_)
				<< "\", \"midiBindings\": \"" << escape_json(po.midiBindings_)
				<< "\"}";
				if(i < list.size() - 1)
				ss << ",";
			}
			ss << "]}";
			return ss.str();
		});
		websocket->sendSessionList();
	}

	if (!pscript->getPolySynth()->getVoices().empty()) {
		if (websocket) {
			websocket->setSetControlCallback([=](string n, float v) {
				for(auto& voice : pscript->getPolySynth()->getVoices()) {
					voice.synth.setParameter(n, v);
				}
			});
			websocket->setNoteOnCallback([=](size_t note, size_t velo) {
				pscript->getPolySynth()->noteOn(note, velo);
			});

			websocket->setNoteOffCallback([=](size_t note) {
				pscript->getPolySynth()->noteOff(note);
			});

			websocket->setClearAllNotesCallback([=]() {
				pscript->getPolySynth()->clearAllNotes();
			});
			websocket->setSendControlListCallback(
					[=]() {
						std::ostringstream ss;
						ss << "{ \"type\": \"control-list\", \"data\": [ ";
						if(pscript->getPolySynth()->getVoices().empty())
						return string("");
						auto params = pscript->getPolySynth()->getVoices()[0].synth.getParameters();
						std::vector<string> parents;
						struct CP {
							float value_;
							float min_;
							float max_;
							string displayName_;
						};
						std::map<string, std::vector<std::pair<string, CP>>> hierachie;
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
							hierachie[parent].push_back( {child, {params[i].getValue(), params[i].getMin(), params[i].getMax(), params[i].getDisplayName()}});
						}

						size_t i = 0;
						for (const auto& parent : parents) {
							const string& module = parent;
							const auto& children = hierachie[parent];
							ss << "{ \"name\": \"" << escape_json(module) << "\", \"controls\": [";
							size_t j = 0;
							for (const auto& child : children) {
								ss << "{ \"name\" :\"" << escape_json(child.first)
								<< "\", \"value\": \"" << child.second.value_
								<< "\", \"min\": \"" << child.second.min_
								<< "\", \"max\": \"" << child.second.max_
								<< "\", \"displayName\": \"" << child.second.displayName_ << "\" }";
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
						return ss.str();
					});

			websocket->setSendStatusReportCallback([=]() {
				std::ostringstream ss;
				ss << "{ \"type\": \"status-report\", \"midiports\": [";
				for(size_t i = 0; i < midiIndex->size(); ++i) {
					const auto& index = (*midiIndex)[i];
					const auto& midiPort = (*midiIn)[i];
					ss << "{ \"index\": " << index
					<< ", \"open\": " << midiPort->isPortOpen()
					<< "}";
					if(i < midiIndex->size() - 1)
					ss << ",";
				}

				ss << "], \"audioport\": "
				<< "{ \"index\": " << params.deviceId
				<< ", \"open\": " << dac->isStreamOpen()
				<< "}";
				ss << "}";
				return ss.str();
			});

			websocket->setUpdateSessionCallback([=](const patchscript::SessionObject& po) {
				if(!isValidSessionName(po.name_)) {
					Logger::getInstance().error("Invalid session name", po.name_);
					return;
				}
				pscript->store(po);
				websocket->sendSessionList();
			});

			websocket->setDeleteSessionCallback([=](const patchscript::SessionObject& po) {
				pscript->remove(po);
				websocket->sendSessionList();
			});

			websocket->reset();
			websocket->sendConfig();
			websocket->sendControlList();
			websocket->sendSessionList();
		}
	}
}
} /* namespace midipatch */
