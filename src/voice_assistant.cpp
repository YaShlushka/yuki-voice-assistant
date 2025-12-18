#include "voice_assistant.h"
#include "common.h"

#include <iostream>

VoiceAssistant::VoiceAssistant(const VoiceAssistantInit& va_init)
	 : recognizer(va_init.model.c_str()) {
	fs::path ctx_file(va_init.ctx_file);
	fs::path often_mistakes(va_init.often_mistakes);
	fs::path websites_links(va_init.websites_links);
	fs::path apps_linux(va_init.apps_linux);
	fs::path apps_windows(va_init.apps_windows);
	fs::path apps_macos(va_init.apps_macos);

	if (!fs::exists(ctx_file)) {
		throw std::runtime_error("Context file does not exists");
	}

	if (!fs::exists(often_mistakes)) {
		throw std::runtime_error("Context file does not exists");
	}

	if (!fs::exists(websites_links)) {
		throw std::runtime_error("File with websites links does not exists");
	}

	if (!fs::exists(apps_linux)) {
		throw std::runtime_error("File with applications for linux does not exists");
	}

	if (!fs::exists(apps_windows)) {
		throw std::runtime_error("File with applications for linux does not exists");
	}

	if (!fs::exists(apps_macos)) {
		throw std::runtime_error("File with applications for linux does not exists");
	}

	ctx_graph_.TrainGraph(va_init.ctx_file);
	ctx_graph_.AddOftenMistakes(va_init.often_mistakes);

	std::ifstream websites_ifs(websites_links);
	const auto& web_links_json = json::Load(websites_ifs).GetRoot();
	if (web_links_json.IsDict()) {
		websites_links_ = web_links_json.AsDict();
	} else {
		throw std::runtime_error("Websites links root is not a dictionary");
	}

	std::ifstream apps_linux_ifs(apps_linux);
	const auto& apps_linux_json = json::Load(apps_linux_ifs).GetRoot();
	if (apps_linux_json.IsDict()) {
		apps_linux_ = apps_linux_json.AsDict();
	} else {
		throw std::runtime_error("Linux applications root is not a dictionary");
	}

	std::ifstream apps_windows_ifs(apps_windows);
	const auto& apps_windows_json = json::Load(apps_windows_ifs).GetRoot();
	if (apps_windows_json.IsDict()) {
		apps_windows_ = apps_windows_json.AsDict();
	} else {
		throw std::runtime_error("Windows applications root is not a dictionary");
	}

	std::ifstream apps_macos_ifs(apps_macos);
	const auto& apps_macos_json = json::Load(apps_macos_ifs).GetRoot();
	if (apps_macos_json.IsDict()) {
		apps_macos_ = apps_macos_json.AsDict();
	} else {
		throw std::runtime_error("MacOs applications root is not a dictionary");
	}
}

void VoiceAssistant::MiniAudioCallback(ma_device* pDevice, void* pOutput, const void* pInput,
													ma_uint32 frame_count) {
	auto* self = static_cast<VoiceAssistant*>(pDevice->pUserData);
	if (!self)
		return;
	self->ProcessAudio(pDevice, pOutput, pInput, frame_count);
}

void VoiceAssistant::ProcessAudio(ma_device* pDevice, void* pOutput, const void* pInput,
											 ma_uint32 frame_count) {
	const int16_t* input_samples = (const int16_t*)pInput;

	for (ma_uint32 i = 0; i < frame_count; i++) {
		audio_buffer.push_back(float(input_samples[i]) / 32768.0f);

		last_speak_time = input_samples[i] > 2000
									 ? 0
									 : (last_speak_time > 5000 ? last_speak_time : last_speak_time + 1);
		is_speak = last_speak_time < 5001 ? true : false;
		is_quiet = last_speak_time < 5001 ? false : is_quiet;
	}

	if (!is_speak && is_quiet) {
		audio_buffer.clear();
	}

	if (!is_speak && !is_quiet) {
		std::cout << recognizer.RecognizeAudio(std::move(audio_buffer)) << std::endl;
		audio_buffer = {};
		is_quiet = true;
	}
}

void VoiceAssistant::ExecRequest(const Request& req) const {}

void VoiceAssistant::OpenReq(const std::string& req) const {
	if (websites_links_.contains(req)) {
		OpenWebSite(websites_links_.at(req).AsString());
		return;
	}
}

void VoiceAssistant::SearchReq(const Request& req) const {}
void VoiceAssistant::TurnOffReq(const Request& req) const {}
void VoiceAssistant::ScreenLockReq(const Request& req) const {}
void VoiceAssistant::ChangeKbLayoutReq(const Request& req) const {}
void VoiceAssistant::StopReq(const Request& req) const {}
void VoiceAssistant::OpenSettingsReq(const Request& req) const {}
