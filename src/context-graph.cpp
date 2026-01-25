#include "context-graph.h"
#include "request.h"

#include <boost/locale.hpp>
#include <rapidfuzz/rapidfuzz/fuzz.hpp>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

const std::string IGNORING_CHARS = ".!?,\"\'-:;";

using CSVLine = std::vector<std::string>;
using CSV = std::vector<CSVLine>;

namespace fs = std::filesystem;

struct Line {
	int id = -1;
	std::string command;
	bool has_arg = true;
};

std::vector<std::string_view> ParseString(std::string_view str) {
	std::vector<std::string_view> result;
	size_t pos = 0;

	while (pos < str.size()) {
		size_t start = str.find_first_not_of(' ', pos);
		if (start == std::string_view::npos) {
			break;
		}

		size_t end = str.find(' ', start);
		if (end == std::string_view::npos) {
			end = str.size();
		}

		result.push_back(str.substr(start, end - start));
		pos = end;
	}
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
	static boost::locale::generator gen;
	static std::locale loc = gen("ru_RU.UTF-8");

	Request req{.type = RequestType::UNKNOWN};
	std::string clean_str;
	for (char ch : phrase) {
		if (IGNORING_CHARS.find(ch) == std::string::npos) {
			clean_str.push_back(std::tolower(ch));
		}
	}

	std::string str = boost::locale::to_lower(clean_str, loc);

	for (const std::pair<std::string, std::string>& mistake : often_mistakes_) {
		size_t pos = str.find(mistake.first);
		size_t start = str.rfind(' ', pos) + 1;
		size_t end = str.find(' ', pos);
		if (pos != std::string::npos) {
			str.replace(start, end - start, mistake.second);
		}
	}

	std::vector<std::string_view> words = ParseString(str);
	std::shared_ptr<Node> cur = graph_;

	for (std::string_view word : words) {
		std::cout << "\"" << word << "\"" << std::endl;
	}

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
				size_t arg_start = str.find(word_str);
				if (arg_start != std::string::npos && arg_start + word_str.size() + 1 < str.size()) {
					req.arg = str.substr(arg_start + word_str.size() + 1);
				} else {
					req.arg = "";
				}
			}
			break;
		}
	}

	return req;
}
