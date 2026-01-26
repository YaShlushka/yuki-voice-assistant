#include "recognize-model.h"

#include <boost/json.hpp>
#include <cstring>
#include <stdexcept>

namespace json = boost::json;

RecognizeModel::RecognizeModel(const char* model_path) : model_(nullptr), recognizer_(nullptr) {
	if (!model_path || std::strlen(model_path) == 0) {
		throw std::runtime_error("Empty Vosk model path");
	}

	model_ = vosk_model_new(model_path);
	if (!model_) {
		throw std::runtime_error("Failed to load Vosk model");
	}

	recognizer_ = vosk_recognizer_new(model_, 16000.f);
	if (!recognizer_) {
		vosk_model_free(model_);
		throw std::runtime_error("Failed to create Vosk recognizer");
	}
}

RecognizeModel::~RecognizeModel() {
	if (recognizer_) {
		vosk_recognizer_free(recognizer_);
		recognizer_ = nullptr;
	}

	if (model_) {
		vosk_model_free(model_);
		model_ = nullptr;
	}
}

std::string RecognizeModel::RecognizeAudio(const std::vector<int16_t>& buf) {
	int final = vosk_recognizer_accept_waveform(
		 recognizer_, reinterpret_cast<const char*>(buf.data()), buf.size() * sizeof(int16_t));

	std::string final_json = vosk_recognizer_final_result(recognizer_);
	auto doc = json::parse(final_json);
	std::string result = "";

	if (doc.as_object().contains("text")) {
		result = doc.as_object().at("text").as_string();
	}

	return result;
}
