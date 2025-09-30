// for check with clang-tidy use clang-tidy main.cpp recognize_model.cpp -- -std=c++20
#include "miniaudio.h"
#include "recognize_model.h"

#include <iostream>
#include <vector>
#include <cstdint>

std::vector<float> audio_buffer;
uint16_t last_speak_time = 5001;
bool is_speak = false;
bool is_quiet = true;

RecognizeModel recognizer("/home/paderinee/Documents/Code/yuki-voice-assistant/models/ggml-base.bin");

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frame_count) {
	const int16_t* input_samples = (const int16_t*)pInput;

	for (ma_uint32 i = 0; i < frame_count; i++) {
		audio_buffer.push_back(float(input_samples[i]) / 32768.0f);

		last_speak_time = input_samples[i] > 1000 ? 0 : (last_speak_time > 5000 ? last_speak_time : last_speak_time + 1);
		is_speak = last_speak_time < 5001 ? true : false;
		is_quiet = last_speak_time < 5001 ? false : is_quiet;
	}

	if(!is_speak && is_quiet) {
		audio_buffer.clear();
	}

	if(!is_speak && !is_quiet) {
		std::cout << recognizer.RecognizeAudio(std::move(audio_buffer)) << std::endl;
		audio_buffer = {};
		is_quiet = true;
	}
}

int main() {
	ma_device_config config = ma_device_config_init(ma_device_type_capture);
	ma_device device;

	config.capture.format = ma_format_s16; // 16 bit
	config.capture.channels = 1;				// mono
	config.sampleRate = 16000;					// 16 kHz
	config.dataCallback = data_callback;	// callback function

	if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
		return -1;
	}

	ma_device_start(&device); // by default device is sleeping
	while(true) {

	}

	ma_device_uninit(&device);
	return 0;
}
