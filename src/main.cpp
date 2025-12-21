// for check with clang-tidy use clang-tidy main.cpp recognize_model.cpp -- -std=c++20
#include "miniaudio/miniaudio.h"
#include "voice_assistant.h"

#include <boost/json.hpp>
#include <cstdlib>
#include <fstream>

namespace json = boost::json;

int main() {
	VoiceAssistantInit va_init;
	std::ifstream settings_ifs("data/settings.json");
	if (!settings_ifs) {
		return EXIT_FAILURE;
	}
	auto settings = json::parse(settings_ifs);
	json::object settings_obj = settings.as_object();
	if (settings_obj.contains("model")) {
		va_init.model = settings_obj.at("model").as_string();
	}
	if (settings_obj.contains("ctx_file")) {
		va_init.ctx_file = settings_obj.at("ctx_file").as_string();
	}
	if (settings_obj.contains("often_mistakes")) {
		va_init.often_mistakes = settings_obj.at("often_mistakes").as_string();
	}
	if (settings_obj.contains("websites")) {
		va_init.websites_links = settings_obj.at("websites").as_string();
	}
	if (settings_obj.contains("apps_linux")) {
		va_init.apps_linux = settings_obj.at("apps_linux").as_string();
	}
	if (settings_obj.contains("apps_windows")) {
		va_init.apps_windows = settings_obj.at("apps_windows").as_string();
	}
	if (settings_obj.contains("apps_macos")) {
		va_init.apps_macos = settings_obj.at("apps_macos").as_string();
	}
	if (va_init.model.empty() || va_init.ctx_file.empty() || va_init.websites_links.empty()) {
		return EXIT_FAILURE;
	}

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

	ma_device_start(&device);
	while (true) {
	}

	ma_device_uninit(&device);
	return EXIT_SUCCESS;
}
