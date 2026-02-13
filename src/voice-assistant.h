#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "context-graph.h"
#include "miniaudio/miniaudio.h"
#include "recognize-model.h"
#include "worker.h"

struct VoiceAssistantInit {
	std::string model;
	std::string ctx_file;
	std::string often_mistakes;
	std::string websites_links;
	std::string applications;
	std::string scenarios;
	std::function<void()> stop_callback;
};

class VoiceAssistant {
 public:
	using Scenario = std::vector<Request>;
	static constexpr int VOL_LIMIT = 2000;
	static constexpr int SAMPLE_RATE = 16000;
	static constexpr double SPEAK_TIME = 1;
	static constexpr int SPEAK_SAMPLES = SAMPLE_RATE * SPEAK_TIME;

	explicit VoiceAssistant(const VoiceAssistantInit& va_init);
	static void MiniAudioCallback(ma_device* p_device, void* p_output, const void* p_input,
											ma_uint32 frame_count);

 private:
	void ProcessAudio(const void* p_input, ma_uint32 frame_count);
	void ExecRequest(const Request& req) const;
	void ExecScenario(const Scenario& scn) const;
	void OpenReq(const std::string& arg) const;

	RecognizeModel recognizer_;
	ContextGraph ctx_graph_;
	std::vector<int16_t> audio_buffer_;
	uint32_t last_speak_time_ = SPEAK_SAMPLES + 1;
	bool is_speak_ = false;
	bool is_processed_ = true;
	std::unordered_map<std::string, std::string> websites_;
	std::unordered_map<std::string, std::string> apps_;
	std::vector<Scenario> scenarios_;
	std::function<void()> stop_callback_;
	Worker worker_;
};
