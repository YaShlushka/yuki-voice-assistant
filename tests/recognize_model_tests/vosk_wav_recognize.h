#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <vosk_api.h>

inline std::vector<int16_t> ReadWAVInt16_t(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("Error opening file: " + filename);
	}

	// Пропускаем WAV header (44 байта)
	file.seekg(44, std::ios::beg);

	std::vector<int16_t> audio_data;
	int16_t sample;

	while (file.read(reinterpret_cast<char*>(&sample), sizeof(int16_t))) {
		audio_data.push_back(sample);
	}

	return audio_data;
}
