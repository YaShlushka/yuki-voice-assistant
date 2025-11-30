#include "vosk_api_recognize_model.h"
#include "vosk_wav_recognize.h"
#include <json/json.h>

#include <iostream>
#include <sstream>
#include <string>

void TestVosk(const std::string& audio_path) {
	vosk::RecognizeModel recognizer("models/vosk-model-ru-0.42");

	for (int i = 1; i <= 10; ++i) {
		for (int t = 1; t <= 31; ++t) {
			std::vector<int16_t> buf =
				 ReadWAV(audio_path + std::to_string(i) + "/" + std::to_string(t) + ".wav");
			std::string str = recognizer.RecognizeAudio(buf);
			std::stringstream sstream(str);
			json::Document doc = json::Load(sstream);

			const json::Dict& root = doc.GetRoot().AsDict();
			if(root.contains("partial")) {
				std::cout << root.at("partial").AsString() << std::endl;
			} else if(root.contains("text")) {
				std::cout << root.at("text").AsString() << std::endl;
			}
		}
	}
}

int main() {
	TestVosk("audio/");
	return 0;
}
