#include "vosk_api_recognize_model.h"
#include "vosk_wav_recognize.h"
#include "whisper_recognize_model.h"
#include "whisper_wav_recognize.h"
#include <json/json.h>

#include <iostream>
#include <string>

void TestVosk(const std::string& audio_path, const std::string& model_path) {
	vosk::RecognizeModel recognizer(model_path);

	for (int i = 1; i <= 10; ++i) {
		for (int t = 1; t <= 30; ++t) {
			std::vector<int16_t> buf =
				 ReadWAVInt16_t(audio_path + std::to_string(i) + "/" + std::to_string(t) + ".wav");
			std::string str = recognizer.RecognizeAudio(buf);
			std::cout << str << std::endl;
		}
	}
}

void TestWhisper(const std::string& audio_path, const std::string& model_path) {
	whisper::RecognizeModel recognizer(model_path);

	for (int i = 1; i <= 10; ++i) {
		for (int t = 1; t <= 30; ++t) {
			std::vector<float> buf =
				 ReadWAVFloat(audio_path + std::to_string(i) + "/" + std::to_string(t) + ".wav");
			std::string str = recognizer.RecognizeAudio(buf);
			std::cout << str << std::endl;
		}
	}
}

int main() {
	TestVosk("audio/", "models/vosk-model-small-ru-0.22");
	TestVosk("audio/", "models/vosk-model-ru-0.42");
	TestWhisper("audio/", "models/ggml-tiny.bin");
	TestWhisper("audio/", "models/ggml-base.bin");
	TestWhisper("audio/", "models/ggml-small.bin");
	TestWhisper("audio/", "models/ggml-medium.bin");
	return 0;
}
