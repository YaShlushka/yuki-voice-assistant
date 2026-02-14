#include "logging.h"
#include "miniaudio/miniaudio.h"
#include "voice-assistant.h"

#include <boost/json.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <atomic>
#include <condition_variable>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mutex>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

namespace json = boost::json;
namespace logging = boost::log;

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
#if defined(_WIN32) || defined(_WIN64)
	SetConsoleOutputCP(65001);
#endif
	try {
		logging::add_common_attributes();
		auto console_sink =
			 logging::add_console_log(std::cout, logging::keywords::format = &JsonFormatter,
											  logging::keywords::auto_flush = true);

		VoiceAssistantInit va_init;
		std::ifstream settings_ifs("data/settings.json");
		if (!settings_ifs) {
			BOOST_LOG_TRIVIAL(error) << "Failed to open data/settings.json";
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
		if (settings_obj.contains("applications")) {
			va_init.applications = settings_obj.at("applications").as_string();
		}
		if (settings_obj.contains("scenarios")) {
			va_init.scenarios = settings_obj.at("scenarios").as_string();
		}
		if (va_init.model.empty() || va_init.ctx_file.empty()) {
			BOOST_LOG_TRIVIAL(error) << "model of context file paths in setting.json are empty";
			return EXIT_FAILURE;
		}
		va_init.stop_callback = StopProgram;

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

		BOOST_LOG_TRIVIAL(info) << "Program loaded successfully";

		std::unique_lock<std::mutex> thread_lock(MTX);
		CV.wait(thread_lock, [] { return STOP_REQUESTED.load(std::memory_order_relaxed); });

		ma_device_stop(&device);
		ma_device_uninit(&device);

		BOOST_LOG_TRIVIAL(info) << "Program was completed successfully";
	} catch (const std::exception& ex) {
		BOOST_LOG_TRIVIAL(error) << logging::add_value(exception, ex.what())
										 << "Unexpected exception catched";
		BOOST_LOG_TRIVIAL(info) << "Program was completed with errors";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
