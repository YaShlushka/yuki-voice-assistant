// for check with clang-tidy use clang-tidy main.cpp recognize_model.cpp -- -std=c++20
#include "miniaudio/miniaudio.h"
#include "voice_assistant.h"

int main() {
	VoiceAssistantInit va_init{.model = "models/ggml-small-q8_0.bin",
										.ctx_file = "data/context_training.csv",
										.often_mistakes = "data/often_mistakes.csv"};
	VoiceAssistant voice_assistant(va_init);

	ma_device_config config = ma_device_config_init(ma_device_type_capture);
	ma_device device;

	config.capture.format = ma_format_s16; // 16 bit
	config.capture.channels = 1;				// mono
	config.sampleRate = 16000;					// 16 kHz
	config.pUserData = &voice_assistant;
	config.dataCallback = VoiceAssistant::MiniAudioCallback; // callback function

	if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
		return -1;
	}

	ma_device_start(&device); // by default device is sleeping
	while (true) {
	}

	ma_device_uninit(&device);
	return 0;
}
