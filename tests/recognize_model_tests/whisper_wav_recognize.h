#pragma once

#include <fstream>
#include <vector>
#include <string>

inline std::vector<float> ReadWAVFloat(const std::string& filename) {
	std::ifstream in(filename, std::ios::binary);
	if (!in)
		throw std::runtime_error("Cannot open file.");

	in.seekg(44, std::ios::beg);
	std::vector<float> audio;
	while (!in.eof()) {
		int16_t sample16;
		in.read(reinterpret_cast<char*>(&sample16), sizeof(sample16));
		if (in.gcount() < sizeof(sample16)) {
			break;
		}
		audio.push_back(sample16 / 32768.0f);
	}
	return audio;
}
