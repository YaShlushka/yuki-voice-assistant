#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include "request.h"

constexpr double ACCURANCY_PERCENT = 70;

struct Node;
using NodePtr = std::shared_ptr<Node>;
using NodeTree = std::unordered_map<std::string_view, NodePtr>;

struct Node {
	RequestType type = RequestType::UNKNOWN;
	bool has_arg = false;
	std::optional<size_t> scenario_id;

	NodeTree childs;
};

class ContextGraph {
 public:
	void AddPhrase(const std::string& phrase, RequestType type, bool has_arg,
						std::optional<size_t> scenario_id = {});
	void TrainGraph(const std::string& file);
	void AddOftenMistakes(const std::string& file);
	Request ParsePhrase(const std::string& str);

 private:
	std::unordered_set<std::string> words_;
	std::shared_ptr<Node> graph_ = std::make_shared<Node>();
	std::unordered_map<std::string, std::string> often_mistakes_;
};
