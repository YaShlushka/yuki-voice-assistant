#pragma once

#include <cstdint>
#include <vector>

#include "context_graph.h"
#include "miniaudio/miniaudio.h"
#include "recognize_model.h"

class VoiceAssistant {
 public:
	explicit VoiceAssistant(const std::string& model, const std::string& ctx_file,
									const std::string& often_mistakes)
		 : recognizer(model.c_str()) {
		ctx_graph_.TrainGraph(ctx_file);
		ctx_graph_.AddOftenMistakes(often_mistakes);
	}

	static void MiniAudioCallback(ma_device* pDevice, void* pOutput, const void* pInput,
											ma_uint32 frame_count);

 private:
	void ProcessAudio(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frame_count);
	void ExecRequest(const Request& req) const;
	void OpenReq(const Request& req) const;
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
};
