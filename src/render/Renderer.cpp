#include "render/Renderer.h"

namespace Renderer {

	Viewport::Shared viewport_;
	Shader::Shared shader_;

	void render(const RenderCommand::Shared& command) {
		// get shader
		auto shader = command->shader;
		if (shader_ != shader) {
			shader->use();
			shader_ = shader;
		}
		// enable state
		for (auto& state : command->enables) {
			glEnable(state);
			LOG_OPENGL("glEnable");
		}
		// state functions
		for (auto& iter : command->functions) {
			auto& func = iter.first;
			auto& args = iter.second;
			switch (func) {
				case FunctionType::BLEND_FUNC:
					glBlendFunc(args[0], args[1]);
					LOG_OPENGL("glBlendFunc");
					break;
				case FunctionType::CULL_FACE:
					glCullFace(args[0]);
					LOG_OPENGL("glCullFace");
					break;
				case FunctionType::DEPTH_MASK:
					glDepthMask(args[0]);
					LOG_OPENGL("glDepthMask");
					break;
			}
		}
		// update viewport
		auto viewport = command->viewport;
		if (viewport) {
			if (!viewport_ || viewport_ != viewport) {
				glViewport(viewport->x, viewport->y, viewport->width, viewport->height);
				LOG_OPENGL("glViewport");
				viewport_ = Viewport::alloc(viewport->x, viewport->y, viewport->width, viewport->height);
			}
		}
		// bind textures
		for (auto& iter : command->textures) {
			auto& location = iter.first;
			auto& texture = iter.second;
			texture->bind(location);
		}
		// bind cubemaps
		for (auto& iter : command->cubemaps) {
			auto& location = iter.first;
			auto& texture = iter.second;
			texture->bind(location);
		}
		// set uniforms
		for (auto& iter : command->uniforms) {
			auto& name = iter.first;
			auto& uniform = iter.second;
			shader->setUniform(name, uniform, true);
		}

		// draw
		command->vao->draw();

		// disable state
		for (auto& state : command->enables) {
			glDisable(state);
			LOG_OPENGL("glDisable");
		}
		// reset state functions
		for (auto& iter : command->functions) {
			auto& func = iter.first;
			switch (func) {
				case FunctionType::BLEND_FUNC:
					glBlendFunc(GL_ONE, GL_ZERO);
					LOG_OPENGL("glBlendFunc");
					break;
				case FunctionType::CULL_FACE:
					glCullFace(GL_BACK);
					LOG_OPENGL("glCullFace");
					break;
				case FunctionType::DEPTH_MASK:
					glDepthMask(GL_TRUE);
					LOG_OPENGL("glDepthMask");
					break;
			}
		}
	}

	void render(const std::vector<RenderCommand::Shared>& commands) {
		for (auto& command : commands) {
			render(command);
		}
	}

}
