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

VoiceAssistant::VoiceAssistant(const VoiceAssistantInit& va_init)
	 : recognizer_(va_init.model.c_str()) {
	fs::path ctx_file(va_init.ctx_file);
	fs::path often_mistakes(va_init.often_mistakes);
	fs::path websites_links(va_init.websites_links);

	if (!fs::exists(ctx_file)) {
		throw std::runtime_error("Context file does not exists");
	}

	if (!fs::exists(often_mistakes)) {
		throw std::runtime_error("Context file does not exists");
	}

	if (!fs::exists(websites_links)) {
		throw std::runtime_error("File with websites links does not exists");
	}

	ctx_graph_.TrainGraph(va_init.ctx_file);
	ctx_graph_.AddOftenMistakes(va_init.often_mistakes);

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

void VoiceAssistant::MiniAudioCallback(ma_device* pDevice, void* pOutput, const void* pInput,
													ma_uint32 frame_count) {
	auto* self = static_cast<VoiceAssistant*>(pDevice->pUserData);
	if (!self) {
		return;
	}

	self->ProcessAudio(pDevice, pOutput, pInput, frame_count);
}

void VoiceAssistant::ProcessAudio(ma_device* pDevice, void* pOutput, const void* pInput,
											 ma_uint32 frame_count) {
	const int16_t* input_samples = (const int16_t*)pInput;

	for (uint32_t i = 0; i < frame_count; i++) {
		audio_buffer_.push_back(input_samples[i]);

		last_speak_time_ = std::abs(input_samples[i]) > 2000
									  ? 0
									  : (last_speak_time_ > 5000 ? last_speak_time_ : last_speak_time_ + 1);
		is_speak_ = last_speak_time_ < 5001 ? true : false;
		is_quiet_ = last_speak_time_ < 5001 ? false : is_quiet_;
	}

	if (!is_speak_ && is_quiet_) {
		audio_buffer_.clear();
	}

	if (!is_speak_ && !is_quiet_) {
		std::string req_str = recognizer_.RecognizeAudio(std::move(audio_buffer_));
		std::cout << req_str << std::endl;
		Request req = ctx_graph_.ParsePhrase(req_str);
		ExecRequest(req);
		audio_buffer_ = {};
		is_quiet_ = true;
	}
}

void VoiceAssistant::ExecRequest(const Request& req) const {
	switch (req.type) {
	case RequestType::OPEN:
		OpenReq(req.arg);
		break;
	case RequestType::WEB_SEARCH:
		SearchReq(req.arg);
		break;
	case RequestType::SHUTDOWN:
		ShutdownReq();
		break;
	case RequestType::SCREEN_LOCK:
		LockScreen();
		break;
	case RequestType::STOP:
		StopReq();
		break;
	case RequestType::UNKNOWN:
		return;
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

void VoiceAssistant::ScreenLockReq(const Request& req) const { LockScreen(); }

void VoiceAssistant::StopReq() const { stop_callback_(); }
