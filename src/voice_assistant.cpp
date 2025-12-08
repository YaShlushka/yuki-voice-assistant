#include "voice_assistant.h"

#include <iostream>

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
