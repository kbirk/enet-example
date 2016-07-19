#pragma once

#include "Common.h"
#include "gl/GLCommon.h"
#include "gl/Uniform.h"
#include "gl/UniformDescriptor.h"
#include "gl/UniformBlockDescriptor.h"

#include <string>
#include <map>

class Shader {

	public:

		typedef std::shared_ptr<Shader> Shared;
		static Shared alloc();

		Shader();
		virtual ~Shader();

		void use() const;

		virtual void setUniform(const std::string&, const Uniform::Shared&, bool);

	protected:

		// prevent copy-construction
		Shader(const Shader&);
		// prevent assignment
		Shader& operator= (const Shader&);

		GLuint id_;
		std::map<std::string, UniformDescriptor::Shared> descriptors_;
		std::map<std::string, UniformBlockDescriptor::Shared> blockDescriptors_;

		bool isGLSL(const std::string&) const;
		std::string readShaderFile(const std::string&) const;
		GLuint createShader(const std::string&, GLenum);
		GLuint createShader(const std::vector<std::string>&, GLenum);
		GLuint createAndCompileShader(const std::string&, GLenum);
		GLuint createAndCompileShader(const std::vector<std::string>&, GLenum);
		bool compileShader(GLuint);
		bool linkProgram();
		void logCompileError(GLuint) const;
		void logLinkError() const;
		std::vector<GLuint> queryUniformIndices() const;
		std::vector<GLint> queryUniformLocations(const std::vector<std::string>&) const;
		std::vector<GLenum> queryUniformTypes(const std::vector<GLuint>&) const;
		std::vector<GLint> queryUniformOffsets(const std::vector<GLuint>&) const;
		std::vector<std::string> queryUniformNames(const std::vector<GLuint>&) const;
		std::vector<GLint> queryParentBlockIndices(const std::vector<GLuint>&) const;
		std::vector<GLuint> queryUniformBlockIndices() const;
		std::vector<GLint> queryUniformBlockSizes(const std::vector<GLuint>&) const;
		std::vector<std::string> queryUniformBlockNames(const std::vector<GLuint>&) const;
		void queryUniforms();
};
