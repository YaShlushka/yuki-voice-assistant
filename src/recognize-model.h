#pragma once

#include <whisper.cpp/include/whisper.h>

#include <vector>
#include <string>

class RecognizeModel {
 public:
	RecognizeModel(const char* model);
	~RecognizeModel();

	std::string RecognizeAudio(std::vector<float> buf);

 private:
	whisper_context_params ctx_params_;
	whisper_context* ctx_;
	whisper_full_params params_;
};
