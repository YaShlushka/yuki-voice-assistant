#include "vosk_api_recognize_model.h"

#include <cstring>
#include <stdexcept>
#include <iostream>

namespace vosk {

RecognizeModel::RecognizeModel(const char* model_path) : model_(nullptr) {
	if (!model_path || std::strlen(model_path) == 0) {
		throw std::runtime_error("Empty Vosk model path");
	}

	model_ = vosk_model_new(model_path);
	if (!model_) {
		throw std::runtime_error("Failed to load Vosk model");
	}

	// recognizer_ = vosk_recognizer_new(model_, 16000.f);
	// if (!recognizer_) {
	// 	throw std::runtime_error("Failed to create recognizer");
	// }
}

RecognizeModel::~RecognizeModel() {
	if (model_) {
		vosk_model_free(model_);
		model_ = nullptr;
	}

	// if (recognizer_) {
	// 	vosk_recognizer_free(recognizer_);
	// 	recognizer_ = nullptr;
	// }
}

std::string RecognizeModel::RecognizeAudio(const std::vector<int16_t>& buf) {
	VoskRecognizer *recognizer_ = vosk_recognizer_new(model_, 16000.f);
	int final = vosk_recognizer_accept_waveform(
		 recognizer_, reinterpret_cast<const char*>(buf.data()), buf.size() * sizeof(int16_t));

	if (final) {
		return vosk_recognizer_result(recognizer_);
	} else {
		return vosk_recognizer_partial_result(recognizer_);
	}
}

} // namespace vosk
