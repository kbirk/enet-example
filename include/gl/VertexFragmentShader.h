#pragma once

#include "Common.h"
#include "gl/Shader.h"
#include "gl/Uniform.h"

#include <memory>
#include <string>

class VertexFragmentShader : public Shader {

	public:

		typedef std::shared_ptr<VertexFragmentShader> Shared;
		static Shared alloc();

		VertexFragmentShader();

		bool create(const std::string& vert, const std::string& frag);
		void setUniform(const std::string&, const Uniform::Shared&, bool = false);
};
