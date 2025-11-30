#pragma once

#include <cstdint>
#include <string>
#include <vector>

// Vosk C API
#include <vosk_api.h>

namespace vosk {

class RecognizeModel {
 public:
	explicit RecognizeModel(const char* model_path);
	~RecognizeModel();

	std::string RecognizeAudio(const std::vector<int16_t>& buf);

 private:
	VoskModel* model_;
	// VoskRecognizer *recognizer_;
};

} // namespace vosk
