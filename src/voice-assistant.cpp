#include "voice-assistant.h"
#include "common.h"
#include "context-graph.h"
#include "request.h"

#include <boost/json.hpp>
#include <rapidfuzz/rapidfuzz/fuzz.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
namespace json = boost::json;

std::vector<std::pair<std::string, VoiceAssistant::Scenario>> LoadScenarios(fs::path path) {
	static auto has_arg = [](RequestType type) {
		int type_int = static_cast<int>(type);
		return type_int == 1 || type_int == 2;
	};

	std::vector<std::pair<std::string, VoiceAssistant::Scenario>> result;
	std::ifstream ifs(path);
	json::value scenarios_val = json::parse(ifs);
	if (!scenarios_val.is_object()) {
		std::cout << "ERROR: Scenarios aren't an object" << std::endl;
		return {};
	}

	json::object scenarios = scenarios_val.as_object();
	for (auto& [name, actions] : scenarios) {
		if (!actions.is_array()) {
			std::cout << "ERROR IN SCENARIOS FILE" << std::endl;
			return {};
		}

		VoiceAssistant::Scenario scn;
		for (auto& action : actions.as_array()) {
			if (!action.is_array() || action.as_array().size() < 1) {
				std::cout << "ERROR WHEN PARSING ACTIONS IN SCENARIOS" << std::endl;
				return {};
			}

			auto& vec = action.as_array();
			if (!vec.at(0).is_int64()) {
				std::cout << "ERROR IN SCENARIOS FILE: command id isn't integer" << std::endl;
				return {};
			}

			RequestType type = static_cast<RequestType>(vec.at(0).as_int64());
			if (has_arg(type) && (vec.size() != 2 || !vec.at(1).is_string())) {
				std::cout << "ERROR WHEN PARSING ACTIONS IN SCENARIOS" << std::endl;
				return {};
			}
			Request req{.type = type,
							.arg = has_arg(type) == true ? std::string(vec.at(1).as_string()) : ""};
			scn.push_back(std::move(req));
		}
		result.push_back({std::move(name), std::move(scn)});
	}

	return result;
}

VoiceAssistant::VoiceAssistant(const VoiceAssistantInit& va_init)
	 : recognizer_(va_init.model.c_str()) {
	fs::path ctx_file(va_init.ctx_file);
	fs::path often_mistakes(va_init.often_mistakes);
	fs::path websites_links(va_init.websites_links);
	fs::path scenarios_path(va_init.scenarios);

	if (!fs::exists(ctx_file)) {
		throw std::runtime_error("Context file does not exists");
	}

	if (!fs::exists(often_mistakes)) {
		std::cout << "Often mistakes file does not exists" << std::endl;
	}

	if (!fs::exists(websites_links)) {
		std::cout << "File with websites links does not exists" << std::endl;
	}

	if (!fs::exists(scenarios_path)) {
		std::cout << "Scenarios file does not exists" << std::endl;
	}

	ctx_graph_.TrainGraph(va_init.ctx_file);
	ctx_graph_.AddOftenMistakes(va_init.often_mistakes);

	std::vector<std::pair<std::string, VoiceAssistant::Scenario>> scenarios =
		 LoadScenarios(scenarios_path);

	for (auto& scenario : scenarios) {
		scenarios_.push_back(std::move(scenario.second));
		ctx_graph_.AddPhrase(scenario.first, RequestType::SCENARIO, false, scenarios_.size() - 1);
	}

	std::ifstream websites_ifs(websites_links);
	auto web_links_json = json::parse(websites_ifs);
	if (web_links_json.is_object()) {
		for (auto& [key, value] : web_links_json.as_object()) {
			websites_[key] = value.as_string();
		}
	} else {
		throw std::runtime_error("Websites links root is not a dictionary");
	}

	fs::path apps_path = va_init.applications;

	std::ifstream apps_ifs(apps_path);
	auto apps_json = json::parse(apps_ifs);
	if (apps_json.is_object()) {
		for (auto& [key, value] : apps_json.as_object()) {
			apps_[key] = value.as_string();
		}
	} else {
		throw std::runtime_error("applications.json root is not a dictionary");
	}

	stop_callback_ = va_init.stop_callback;
}

void VoiceAssistant::MiniAudioCallback(ma_device* p_device, void* p_output, const void* p_input,
													ma_uint32 frame_count) {
	auto* self = static_cast<VoiceAssistant*>(p_device->pUserData);
	if (!self) {
		return;
	}

	self->ProcessAudio(p_input, frame_count);
}

void VoiceAssistant::ProcessAudio(const void* p_input, ma_uint32 frame_count) {
	const int16_t* input_samples = static_cast<const int16_t*>(p_input);

	for (uint32_t i = 0; i < frame_count; i++) {
		last_speak_time_ =
			 std::abs(input_samples[i]) > VOL_LIMIT
				  ? 0
				  : (last_speak_time_ > SPEAK_SAMPLES ? last_speak_time_ : last_speak_time_ + 1);
		is_speak_ = last_speak_time_ < SPEAK_SAMPLES + 1 ? true : false;
		is_processed_ = last_speak_time_ < SPEAK_SAMPLES + 1 ? false : is_processed_;

		if (is_speak_ || !is_processed_) {
			audio_buffer_.push_back(input_samples[i]);
		}
	}

	if (!is_speak_ && !is_processed_) {
		worker_.AddTask([this, audio_buffer = std::move(audio_buffer_)] {
			std::string req_str = recognizer_.RecognizeAudio(std::move(audio_buffer));
			std::cout << req_str << std::endl;
			Request req = ctx_graph_.ParsePhrase(req_str);
			ExecRequest(req);
		});

		audio_buffer_ = {};
		is_processed_ = true;
	}
}

void VoiceAssistant::ExecRequest(const Request& req) const {
	switch (req.type) {
	case RequestType::SCENARIO:
		ExecScenario(scenarios_[*req.scenario_id]);
		break;
	case RequestType::OPEN:
		OpenReq(req.arg);
		break;
	case RequestType::WEB_SEARCH:
		SearchReq(req.arg);
		break;
	case RequestType::SHUTDOWN:
		ShutdownReq();
		break;
	case RequestType::STOP:
		StopReq();
		break;
	case RequestType::TOGGLE_MEDIA:
		ToggleMedia();
		break;
	case RequestType::PREVIOUS_MEDIA:
		PreviousMedia();
		break;
	case RequestType::NEXT_MEDIA:
		NextMedia();
		break;
	case RequestType::UNKNOWN:
		return;
	}
}

void VoiceAssistant::ExecScenario(const Scenario& scn) const {
	for (const Request& req : scn) {
		ExecRequest(req);
	}
}

void VoiceAssistant::OpenReq(const std::string& arg) const {
	if (apps_.contains(arg)) {
		OpenApplication(apps_.at(arg));
		return;
	}

	if (websites_.contains(arg)) {
		OpenWebSite(websites_.at(arg));
		return;
	}

	for (const auto& app_pair : apps_) {
		if (rapidfuzz::fuzz::ratio(arg, app_pair.first) >= ACCURANCY_PERCENT) {
			OpenApplication(app_pair.second);
			return;
		}
	}

	for (const auto& web_pair : websites_) {
		if (rapidfuzz::fuzz::ratio(arg, web_pair.first) >= ACCURANCY_PERCENT) {
			OpenWebSite(web_pair.second);
			return;
		}
	}
}

void VoiceAssistant::SearchReq(const std::string& arg) const { SearchOnTheInternet(arg); }

void VoiceAssistant::ShutdownReq() const { Shutdown(); }

void VoiceAssistant::StopReq() const { stop_callback_(); }
