#include "../src/neural-network.h"

#include <algorithm>
#include <boost/json.hpp>
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace json = boost::json;

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

int main() {
	std::map<std::string, int> dict;
	int last_id = 0;
	std::vector<NeuralNetwork::TrainingData> td;
	std::ifstream ifs("data/neural-netwok-training-data.json");
	if (!ifs) {
		throw std::runtime_error("File for training neural network does not exist");
	}

	json::object obj = json::parse(ifs).as_object();

	// for (const auto& [text, command_id] : obj) {
	// 	std::vector<std::string_view> parsed_str = ParseString(text);
	// 	for (std::string_view word : parsed_str) {
	// 		std::string word_str = std::string(word);
	// 		if (!dict.contains(word_str)) {
	// 			dict.insert({std::move(word_str), last_id++});
	// 		}
	// 	}
	// }
	//
	// for (const auto& [text, command_id] : obj) {
	// 	std::vector<std::string_view> parsed_str = ParseString(text);
	// 	NeuralNetwork::InputElem ie(dict.size());
	// 	NeuralNetwork::OutputElem oe(6);
	// 	oe[command_id.as_int64()] = 1;
	// 	for (std::string_view word : parsed_str) {
	// 		std::string word_str = std::string(word);
	// 		ie[dict.at(word_str)] = 1;
	// 	}
	// 	td.push_back({ie, oe});
	// }

	for (const auto& [id, command_arr] : obj) {
		json::array commands = command_arr.as_array();
		for (const json::value& command : commands) {
			std::string str = std::string(command.as_string());
			std::vector<std::string_view> parsed_str = ParseString(str);
			for (std::string_view word : parsed_str) {
				std::string word_str = std::string(word);
				if (!dict.contains(word_str)) {
					dict.insert({std::move(word_str), last_id++});
				}
			}
		}
	}

	for (const auto& [id, command_arr] : obj) {
		int command_id = std::stoi(id);
		json::array commands = command_arr.as_array();
		for (const json::value& command : commands) {
			std::string str = std::string(command.as_string());
			std::vector<std::string_view> parsed_str = ParseString(str);
			NeuralNetwork::InputElem ie(dict.size());
			NeuralNetwork::OutputElem oe(6);

			for(double& i : ie) {
				i = 0;
			}

			for(double& i : oe) {
				i = 0;
			}
			oe[command_id] = 1;
			for (std::string_view word : parsed_str) {
				std::string word_str = std::string(word);
				ie[dict.at(word_str)] = 1;
			}
			td.push_back({ie, oe});
		}
	}

	NeuralNetwork net({static_cast<int>(dict.size()), 100, 70, 50, 6});
	net.SGD(td, 2000, 10, 0.05, td);

	std::string test_phrase = "Юки, открой ютуб";
	NeuralNetwork::InputElem ie(static_cast<int>(dict.size()));
	std::vector<std::string_view> parsed_phrase = ParseString(test_phrase);
	for (std::string_view word : parsed_phrase) {
		std::string word_str = std::string(word);
		if (dict.contains(word_str)) {
			ie[dict.at(word_str)] = 1;
		}
	}

	auto res = net.FeedForward(ie);
	std::cout << std::distance(res.begin(), std::max_element(res.begin(), res.end())) << std::endl;
}
