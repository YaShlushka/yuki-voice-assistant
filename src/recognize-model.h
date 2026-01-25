#pragma once

// #include <whisper.cpp/include/whisper.h>
#include <vosk_api.h>

#include <vector>
#include <string>

class RecognizeModel {
 public:
	RecognizeModel(const char* model);
	~RecognizeModel();

	std::string RecognizeAudio(const std::vector<int16_t>& buf);

 private:
	// whisper_context_params ctx_params_;
	// whisper_context* ctx_;
	// whisper_full_params params_;
	VoskModel* model_;
};
