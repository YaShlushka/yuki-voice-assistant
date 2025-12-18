#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <vector>

#include "context_graph.h"
#include "miniaudio/miniaudio.h"
#include "recognize_model.h"

#include <json/json.h>

namespace fs = std::filesystem;

struct VoiceAssistantInit {
	std::string model;
	std::string ctx_file;
	std::string often_mistakes;
	std::string websites_links;
	std::string apps_linux;
	std::string apps_windows;
	std::string apps_macos;
};

class VoiceAssistant {
 public:
	explicit VoiceAssistant(const VoiceAssistantInit& va_init);
	static void MiniAudioCallback(ma_device* pDevice, void* pOutput, const void* pInput,
											ma_uint32 frame_count);

 private:
	void ProcessAudio(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frame_count);
	void ExecRequest(const Request& req) const;
	void OpenReq(const std::string& arg) const;
	void SearchReq(const Request& req) const;
	void TurnOffReq(const Request& req) const;
	void ScreenLockReq(const Request& req) const;
	void ChangeKbLayoutReq(const Request& req) const;
	void StopReq(const Request& req) const;
	void OpenSettingsReq(const Request& req) const;

	RecognizeModel recognizer;
	ContextGraph ctx_graph_;
	std::vector<float> audio_buffer;
	uint16_t last_speak_time = 5001;
	bool is_speak = false;
	bool is_quiet = true;
	json::Dict websites_links_;
	json::Dict apps_linux_;
	json::Dict apps_windows_;
	json::Dict apps_macos_;
};
