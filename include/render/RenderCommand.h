#pragma once

#include "Common.h"
#include "gl/Shader.h"
#include "gl/Texture2D.h"
#include "gl/TextureCubeMap.h"
#include "gl/Uniform.h"
#include "gl/Viewport.h"
#include "gl/VertexArrayObject.h"

#include <vector>
#include <map>

enum class FunctionType {
	BLEND_FUNC,
	CULL_FACE,
	DEPTH_MASK
	//BLEND_COLOR,
	//BLEND_EQUATIONS,
};

class RenderCommand {

	public:

		typedef std::shared_ptr<RenderCommand> Shared;
		static Shared alloc();

		RenderCommand();

		void merge(const RenderCommand::Shared&);

		std::vector<GLenum> enables;
		std::vector<GLenum> disables;
		std::map<FunctionType, std::vector<GLenum>> functions;
		std::map<std::string, Uniform::Shared> uniforms;
		std::map<GLenum, Texture2D::Shared> textures;
		std::map<GLenum, TextureCubeMap::Shared> cubemaps;
		VertexArrayObject::Shared vao;
		Shader::Shared shader;
	 	Viewport::Shared viewport;

	private:

		// prevent copy-construction
		RenderCommand(const RenderCommand&);
		// prevent assignment
		RenderCommand& operator= (const RenderCommand&);
};
