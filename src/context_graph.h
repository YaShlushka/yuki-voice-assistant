#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "request.h"

struct Node;
using NodePtr = std::shared_ptr<Node>;
using NodeTree = std::unordered_map<std::string, NodePtr>;

struct Node {
	RequestType type = RequestType::UNKNOWN;
	bool has_arg = false;

	NodeTree childs;
};

class ContextGraph {
 public:
	void TrainGraph(const std::string& file);
	void AddOftenMistakes(const std::string& file);
	Request ParsePhrase(const std::string& str);

 private:
	std::shared_ptr<Node> graph_ = std::make_shared<Node>();
	std::unordered_map<std::string, std::string> often_mistakes_;
};
