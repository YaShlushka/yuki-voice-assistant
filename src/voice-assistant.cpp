#include "voice-assistant.h"
#include "common.h"
#include "context-graph.h"
#include "logging.h"
#include "request.h"

#include <boost/json.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <rapidfuzz/rapidfuzz/fuzz.hpp>

#include <filesystem>
#include <fstream>

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
		BOOST_LOG_TRIVIAL(error) << logging::add_value(where, "VoiceAssistant init")
										 << "Scenarios must be json object";
		return {};
	}

	json::object scenarios = scenarios_val.as_object();
	for (auto& [name, actions] : scenarios) {
		if (!actions.is_array()) {
			BOOST_LOG_TRIVIAL(error) << logging::add_value(where, "VoiceAssistant init")
											 << "Actions for scenario must be array";
			return {};
		}

		VoiceAssistant::Scenario scn;
		for (auto& action : actions.as_array()) {
			if (!action.is_array() || action.as_array().size() < 1) {
				BOOST_LOG_TRIVIAL(error) << logging::add_value(where, "VoiceAssistant init")
												 << "Action in actions array must be array";
				return {};
			}

			auto& vec = action.as_array();
			if (!vec.at(0).is_int64()) {
				BOOST_LOG_TRIVIAL(error) << logging::add_value(where, "VoiceAssistant init")
												 << "Command id in action must be integer";
				return {};
			}

			RequestType type = static_cast<RequestType>(vec.at(0).as_int64());
			if (has_arg(type) && (vec.size() != 2 || !vec.at(1).is_string())) {
				BOOST_LOG_TRIVIAL(error)
					 << logging::add_value(where, "VoiceAssistant init")
					 << "Error parsing argument for command: the argument is missing or not string";
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
		throw std::runtime_error(va_init.ctx_file + " does not exists");
	}

	if (!fs::exists(often_mistakes)) {
		BOOST_LOG_TRIVIAL(warning) << va_init.often_mistakes + " does not exists";
	}

	if (!fs::exists(websites_links)) {
		BOOST_LOG_TRIVIAL(warning) << va_init.websites_links + " does not exists";
	}

	if (!fs::exists(scenarios_path)) {
		BOOST_LOG_TRIVIAL(warning) << va_init.scenarios + " does not exists";
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
		BOOST_LOG_TRIVIAL(error) << logging::add_value(where, "VoiceAssistant init")
										 << va_init.websites_links + " root is not a dictionary";
	}

	fs::path apps_path = va_init.applications;

	std::ifstream apps_ifs(apps_path);
	auto apps_json = json::parse(apps_ifs);
	if (apps_json.is_object()) {
		for (auto& [key, value] : apps_json.as_object()) {
			apps_[key] = value.as_string();
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << logging::add_value(where, "VoiceAssistant init")
										 << va_init.applications + " root is not a dictionary";
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
		BOOST_LOG_TRIVIAL(info) << "Adding request to queue for processing";
		worker_.AddTask([this, audio_buffer = std::move(audio_buffer_)] {
			std::string req_str = recognizer_.RecognizeAudio(std::move(audio_buffer));
			BOOST_LOG_TRIVIAL(info) << "Audio recognized: " + req_str;
			Request req = ctx_graph_.ParsePhrase(req_str);
			ExecRequest(req);
		});

		audio_buffer_ = {};
		is_processed_ = true;
	}
}

void VoiceAssistant::ExecRequest(const Request& req) const {
	std::string type = GetRequestTypeString(req.type);
	std::string arg = req.arg;
	std::optional<size_t> scn_id = req.scenario_id;

	if (!arg.empty()) {
		BOOST_LOG_TRIVIAL(info) << logging::add_value(request_type, type)
										<< logging::add_value(request_arg, req.arg);
	} else if (scn_id) {
		BOOST_LOG_TRIVIAL(info) << logging::add_value(request_type, type)
										<< logging::add_value(scenario_id, *scn_id);
	} else {
		BOOST_LOG_TRIVIAL(info) << logging::add_value(request_type, type);
	}

	switch (req.type) {
	case RequestType::SCENARIO:
		ExecScenario(scenarios_[*req.scenario_id]);
		break;
	case RequestType::OPEN:
		OpenReq(req.arg);
		break;
	case RequestType::WEB_SEARCH:
		SearchOnTheInternet(req.arg);
		break;
	case RequestType::SHUTDOWN:
		Shutdown();
		break;
	case RequestType::STOP:
		stop_callback_();
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

	{
		double max_percent = 0;
		std::string res;
		for (const auto& app_pair : apps_) {
			double percent = rapidfuzz::fuzz::ratio(arg, app_pair.first);
			if (percent >= ACCURANCY_PERCENT && percent > max_percent) {
				res = app_pair.second;
				max_percent = percent;
			}
		}

		if (!res.empty()) {
			OpenApplication(res);
			return;
		}
	}

	{
		double max_percent = 0;
		std::string res;
		for (const auto& web_pair : websites_) {
			double percent = rapidfuzz::fuzz::ratio(arg, web_pair.first);
			if (percent >= ACCURANCY_PERCENT && percent > max_percent) {
				res = web_pair.second;
				max_percent = percent;
			}
		}

		if (!res.empty()) {
			OpenWebSite(res);
			return;
		}
	}
}
