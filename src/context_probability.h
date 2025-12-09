#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "request.h"

struct Node;
using NodeTree = std::unordered_map<std::string, std::shared_ptr<Node>>;

struct Node {
	double confidence;
	RequestType type;

	NodeTree childs;
};

class ContextProbability {
 public:
	void TrainGraph(const std::string& file);

 private:
	NodeTree nodes_;
};
