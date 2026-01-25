#include "miniaudio/miniaudio.h"
#include "voice-assistant.h"

#include <boost/json.hpp>

#include <cstdlib>
#include <fstream>
#include <atomic>
#include <condition_variable>
#include <csignal>
#include <mutex>
#include <iostream>

namespace json = boost::json;

namespace {

std::atomic_bool STOP_REQUESTED{false};
std::mutex MTX;
std::condition_variable CV;

void StopProgram() {
	STOP_REQUESTED.store(true, std::memory_order_relaxed);
	CV.notify_one();
}

void SignalHandler(int) { StopProgram(); }

} // namespace

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
	config.dataCallback = VoiceAssistant::MiniAudioCallback;

	if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
		return -1;
	}

	if (ma_device_start(&device) != MA_SUCCESS) {
		ma_device_uninit(&device);
		return -1;
	}

	std::signal(SIGINT, SignalHandler);
	std::signal(SIGTERM, SignalHandler);

	std::cout << "START" << std::endl;

	std::unique_lock<std::mutex> thread_lock(MTX);
	CV.wait(thread_lock, [] { return STOP_REQUESTED.load(std::memory_order_relaxed); });

	ma_device_stop(&device);
	ma_device_uninit(&device);

	return EXIT_SUCCESS;
}
