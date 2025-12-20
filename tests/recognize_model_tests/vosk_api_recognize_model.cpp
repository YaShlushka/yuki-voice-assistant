#include "vosk_api_recognize_model.h"

#include <cstring>
#include <boost/json.hpp>
#include <sstream>
#include <stdexcept>

namespace vosk {

namespace json = boost::json;

RecognizeModel::RecognizeModel(const std::string& model_path) : model_(nullptr) {
	if (!model_path.c_str() || std::strlen(model_path.c_str()) == 0) {
		throw std::runtime_error("Empty Vosk model path");
	}

	model_ = vosk_model_new(model_path.c_str());
	if (!model_) {
		throw std::runtime_error("Failed to load Vosk model");
	}
}

RecognizeModel::~RecognizeModel() {
	if (model_) {
		vosk_model_free(model_);
		model_ = nullptr;
	}
}

std::string RecognizeModel::RecognizeAudio(const std::vector<int16_t>& buf) {
	VoskRecognizer* recognizer = vosk_recognizer_new(model_, 16000.f);
	int final = vosk_recognizer_accept_waveform(
		 recognizer, reinterpret_cast<const char*>(buf.data()), buf.size() * sizeof(int16_t));

	std::stringstream sstream;
	if (final) {
		sstream << vosk_recognizer_result(recognizer);
	} else {
		sstream << vosk_recognizer_partial_result(recognizer);
	}

	vosk_recognizer_free(recognizer);

	auto doc = json::parse(sstream);

	std::string result;
	if (doc.as_object().contains("partial")) {
		result = doc.as_object().at("partial").as_string();
	} else if (doc.as_object().contains("text")) {
		result = doc.as_object().at("text").as_string();
	}

	return "";
}

} // namespace vosk
