#pragma once

#include "Common.h"
#include "gl/GLCommon.h"

#include <memory>
#include <vector>

class VertexBufferObject {

	public:

		typedef std::shared_ptr<VertexBufferObject> Shared;
		static Shared alloc(
			GLenum mode = GL_TRIANGLES,
			GLuint count = 0,
			GLuint byteOffset = 0);

		explicit
		VertexBufferObject(
			GLenum mode = GL_TRIANGLES,
			GLuint count = 0,
			GLuint byteOffset = 0);

		~VertexBufferObject();

		template<typename T>
		void upload(const std::vector<T>&, GLenum usage = GL_STATIC_DRAW);

		void bind() const;
		void unbind() const;
		void draw() const;

	private:

		// prevent copy-construction
		VertexBufferObject(const VertexBufferObject&);
		// prevent assignment
		VertexBufferObject& operator= (const VertexBufferObject&);

		GLuint id_;
		GLenum mode_;
		GLuint count_;
		GLuint byteOffset_;
};

template<typename T>
void VertexBufferObject::upload(const std::vector<T>& data, GLenum usage) {
	// if buffer not allocated, generate
	if (!id_) {
		glGenBuffers(1, &id_);
		LOG_OPENGL("glGenBuffers");
	}
	// get buffer size
	const size_t bufferSize = data.size();
	// update count if it is zero
	if (count_ == 0) {
		count_ = bufferSize;
	}
	// bind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, id_);
	LOG_OPENGL("glBindBuffer");
	// buffer the data
	glBufferData(GL_ARRAY_BUFFER, bufferSize * sizeof(T), &data[0], usage);
	LOG_OPENGL("glBufferData");
	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	LOG_OPENGL("glBindBuffer");
}
