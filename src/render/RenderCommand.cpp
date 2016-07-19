#include "render/RenderCommand.h"

#include <algorithm>

RenderCommand::Shared RenderCommand::alloc() {
	return std::make_shared<RenderCommand>();
}

RenderCommand::RenderCommand() {
}

void RenderCommand::merge(const RenderCommand::Shared& command) {
	for (auto enable : command->enables) {
		auto iter = std::find(enables.begin(), enables.end(), enable);
		if (iter == enables.end()) {
			enables.push_back(enable);
		}
	}
	for (auto iter : command->functions) {
		functions[iter.first] = iter.second;
	}
	for (auto iter : command->uniforms) {
		uniforms[iter.first] = iter.second;
	}
	for (auto iter : command->textures) {
		textures[iter.first] = iter.second;
	}
	if (command->vao) {
		vao = command->vao;
	}
	if (command->shader) {
		shader = command->shader;
	}
	if (command->viewport) {
		viewport = command->viewport;
	}
}
