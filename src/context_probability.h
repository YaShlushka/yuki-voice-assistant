#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "request.h"

struct Node;

// struct NodeHasher {
// 	size_t operator()(const std::shared_ptr<Node>& node) const;
// };

// bool operator==(const std::shared_ptr<Node>& v1, const std::shared_ptr<Node>& v2);

struct Node {
	double confidence;
	RequestType type;

	std::unordered_map<std::string, std::shared_ptr<Node>> childs;
};

class ContextProbability {
 public:
 private:
	std::unordered_map<std::string, Node> nodes_;
};
