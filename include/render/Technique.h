#pragma once

#include "Common.h"
#include "gl/Shader.h"
#include "gl/Viewport.h"
#include "gl/Uniform.h"
#include "render/RenderCommand.h"

#include <map>
#include <memory>
#include <vector>

class Technique {

	public:

		typedef std::shared_ptr<Technique> Shared;
		static Shared alloc();

		Technique();

		void setShader(Shader::Shared);
		void setUniform(const std::string&, Uniform::Shared);
		void setFunction(FunctionType, const std::vector<GLenum>&);
		void setViewport(Viewport::Shared);
		void enable(GLenum);

		RenderCommand::Shared render() const;

	private:

		// prevent copy-construction
		Technique(const Technique&);
		// prevent assignment
		Technique& operator= (const Technique&);

		Shader::Shared shader_;
		Viewport::Shared viewport_;
		std::vector<GLenum> enables_;
		std::map<FunctionType, std::vector<GLenum>> functions_;
		std::map<std::string, Uniform::Shared> uniforms_;
};
