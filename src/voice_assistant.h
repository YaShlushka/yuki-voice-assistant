#pragma once

#include <cstdint>
#include <vector>

#include "miniaudio/miniaudio.h"
#include "recognize_model.h"

class VoiceAssistant {
 public:
	explicit VoiceAssistant(const std::string& model) : recognizer(model.c_str()) {}

	static void MiniAudioCallback(ma_device* pDevice, void* pOutput, const void* pInput,
											ma_uint32 frame_count);

 private:
	void ProcessAudio(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frame_count);

	RecognizeModel recognizer;
	std::vector<float> audio_buffer;
	uint16_t last_speak_time = 5001;
	bool is_speak = false;
	bool is_quiet = true;
};
