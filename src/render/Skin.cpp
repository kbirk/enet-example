#include "render/Skin.h"

Skin::Shared Skin::alloc() {
	return std::make_shared<Skin>();
}

Skin::Skin() {
}

void Skin::addNode(Node::Shared node, const glm::mat4& inverseBind) {
	nodes_.push_back(node);
	inverseBindMatrices_.push_back(inverseBind); // bone->mOffsetMatrix
	indices_[node->name()] = indices_.size();
}

int32_t Skin::getNodeIndex(const std::string& name) const {
	auto iter = indices_.find(name);
	if (iter == indices_.end()) {
		return -1;
	}
	return iter->second;
}
