#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "request.h"

struct Node;
using NodePtr  = std::shared_ptr<Node>;
using NodeTree = std::unordered_map<std::string, NodePtr>;

struct Node {
	RequestType type;
	bool has_arg = false;

	NodeTree childs;
};

class ContextGraph {
 public:
	void TrainGraph(const std::string& file);
	Request ParsePhrase(const std::string& str);

 private:
	NodeTree graph_;
};
