#include "context_graph.h"
#include "request.h"
#include <memory>
#include <rapidfuzz/rapidfuzz/fuzz.hpp>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

const std::string IGNORING_CHARS = ".!?,\"\'-:;";
constexpr double ACCURANCY_PERCENT = 70;

using CSVLine = std::vector<std::string>;
using CSV = std::vector<CSVLine>;

namespace fs = std::filesystem;

struct Line {
	int id = -1;
	std::string command;
	bool has_arg = true;
};

std::vector<std::string_view> ParseString(std::string_view str) {
	size_t begin = 0;
	size_t end = 0;
	std::vector<std::string_view> result;
	while ((end = str.find(' ', begin)) != std::string_view::npos) {
		result.push_back(str.substr(begin, end - begin));
		begin = end + 1;
	}
	result.push_back(str.substr(begin));

	return result;
}

CSVLine ParseCSVLine(const std::string& line) {
	size_t begin = 0;
	size_t end = 0;
	CSVLine result;
	while ((end = line.find(',', begin)) != std::string::npos) {
		result.push_back(line.substr(begin, end - begin));
		begin = end + 1;
	}
	result.push_back(line.substr(begin));

	return result;
}

CSV ReadCSV(const std::string& file) {
	fs::path path(file);
	if (!fs::exists(file)) {
		return {};
	}

	std::ifstream ifs(path);
	if (!ifs) {
		return {};
	}

	CSV result;
	std::string line;
	std::getline(ifs, line);
	while (std::getline(ifs, line)) {
		CSVLine vec = ParseCSVLine(line);
		result.push_back(std::move(vec));
	}
	ifs.close();

	return result;
}

void ContextGraph::TrainGraph(const std::string& file) {
	CSV lines = ReadCSV(file);
	if (lines.empty()) {
		throw std::runtime_error("Error reading training csv file");
	}

	for (const std::vector<std::string>& line : lines) {
		assert(line.size() == 3);
		std::shared_ptr<Node> cur = graph_;
		std::vector<std::string_view> words = ParseString(line[1]);
		for (size_t i = 0; i < words.size(); ++i) {
			std::string word_str = std::string(words[i]);
			if (cur->childs.contains(word_str)) {
				cur = cur->childs.at(word_str);
			} else {
				auto new_node = std::make_shared<Node>();
				new_node->type = (i == (words.size() - 1))
											? (static_cast<RequestType>(std::stoi(line[0])))
											: (RequestType::UNKNOWN);

				new_node->childs = NodeTree();
				new_node->has_arg = line[2] == "arg";

				cur->childs.emplace(std::move(word_str), new_node);
				cur = new_node;
			}
		}
	}
}

void ContextGraph::AddOftenMistakes(const std::string& file) {
	CSV mistakes = ReadCSV(file);
	for (const CSVLine& line : mistakes) {
		assert(line.size() == 2);
		often_mistakes_.insert({line[0], line[1]});
	}
}

Request ContextGraph::ParsePhrase(const std::string& phrase) {
	using namespace rapidfuzz;

	Request req;
	std::string str;
	for (char ch : phrase) {
		if (IGNORING_CHARS.find(ch) == std::string::npos) {
			str.push_back(std::tolower(ch));
		}
	}

	for (const std::pair<std::string, std::string>& mistake : often_mistakes_) {
		size_t pos = str.find(mistake.first);
		if (pos != std::string::npos) {
			str.replace(pos, mistake.first.size(), mistake.second);
		}
	}

	std::vector<std::string_view> words = ParseString(str);
	std::shared_ptr<Node> cur = graph_;
	for (std::string_view word : words) {
		std::string word_str = std::string(word);
		if (cur->childs.contains(word_str)) {
			cur = cur->childs.at(word_str);
		} else {
			for (const auto& pair : cur->childs) {
				if (fuzz::ratio(pair.first, word_str) >= ACCURANCY_PERCENT) {
					cur = cur->childs.at(pair.first);
				}
				break;
			}
		}

		if (cur->type != RequestType::UNKNOWN) {
			req.type = cur->type;

			if (cur->has_arg) {
				req.arg = str.substr(str.find(word_str) + word.size() + 1);
			}
			break;
		}
	}

	return req;
}
