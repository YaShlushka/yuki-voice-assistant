#pragma once

#include <vosk_api.h>

#include <vector>
#include <string>
#include <cstdint>

class RecognizeModel {
 public:
	RecognizeModel(const char* model);
	~RecognizeModel();

	std::string RecognizeAudio(const std::vector<int16_t>& buf);

 private:
	VoskModel* model_;
	VoskRecognizer* recognizer_;
};
