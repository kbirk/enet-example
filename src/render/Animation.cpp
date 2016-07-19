#include "render/Animation.h"

#include <algorithm>

Animation::Shared Animation::alloc() {
	return std::make_shared<Animation>();
}

Animation::Animation() {
}

void Animation::addNode(const std::string& name, Node::Shared node) {
	nodes_[name] = node;
	indices_.push_back(name);
}

std::vector<Node::Shared> Animation::nodes() {
	std::vector<Node::Shared> nodes(indices_.size(), 0);
	for (uint32_t i=0; i<indices_.size(); i++) {
		nodes[i] = nodes_[indices_[i]];
	}
	return nodes;
}

std::vector<uint32_t> Animation::indices() {
	std::vector<uint32_t> indices(indices_.size(), 0);
	for (uint32_t i=0; i<indices_.size(); i++) {
		indices[i] = i;
	}
	return indices;
}
