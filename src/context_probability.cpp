#include "context_probability.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;

struct Line {
	int id = -1;
	std::string command;
	bool has_arg = true;
};

std::vector<std::string_view> ParseStringBySpaces(std::string_view str) {
	size_t begin = 0;
	size_t end = 0;
	std::vector<std::string_view> result;
	while ((end = str.find(' ', begin)) != std::string_view::npos) {
		result.push_back(str.substr(begin, end - begin - 1));
		begin = end + 1;
	}
	result.push_back(str.substr(begin));

	return result;
}

std::vector<std::string> ParseCSVLine(const std::string& line) {
	size_t begin = 0;
	size_t end = 0;
	std::vector<std::string> result;
	while ((end = line.find(',', begin)) != std::string::npos) {
		result.push_back(line.substr(begin, end - begin));
		begin = end + 1;
	}
	result.push_back(line.substr(begin));

	return result;
}

std::vector<Line> ReadCSV(const std::string& file) {
	fs::path path(file);
	if (!fs::exists(file)) {
		return {};
	}

	std::ifstream ifs(path);
	if (!ifs) {
		return {};
	}

	std::vector<Line> result;
	std::string line;
	std::getline(ifs, line);
	while (std::getline(ifs, line)) {
		std::vector<std::string> vec = ParseCSVLine(line);
		Line l;
		l.id = std::stoi(vec[0]);
		l.command = vec[1];
		l.has_arg = vec[2] == "arg";
		result.push_back(std::move(l));
	}
	ifs.close();

	return result;
}

void ContextProbability::TrainGraph(const std::string& file) {
	NodeTree graph;
	std::vector<Line> lines = ReadCSV(file);
	if (lines.empty()) {
		throw std::runtime_error("Error reading training csv file");
	}

	for (const Line& line : lines) {
		std::shared_ptr<NodeTree> cur = std::make_shared<NodeTree>(graph);
		std::vector<std::string_view> words = ParseStringBySpaces(line.command);
		for (std::string_view word : words) {
			std::string word_str = std::string(word);
			if (cur->contains(word_str)) {
				cur = std::make_shared<NodeTree>(cur->at(word_str)->childs);
			} else {

			}
		}
	}
}
