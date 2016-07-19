#pragma once

#include "Common.h"
#include "gl/GLCommon.h"

#include <memory>

class VertexAttributePointer {

	public:

		typedef std::shared_ptr<VertexAttributePointer> Shared;
		static Shared alloc(
			GLuint index,
			GLuint size,
			GLuint type,
			GLuint byteOffset = 0,
			GLuint stride = 0);

		VertexAttributePointer(
			GLuint index,
			GLuint size,
			GLuint type,
			GLuint byteOffset = 0,
			GLuint stride = 0);

		void bind() const;
		void unbind() const;

	private:

		// prevent copy-construction
		VertexAttributePointer(const VertexAttributePointer&);
		// prevent assignment
		VertexAttributePointer& operator= (const VertexAttributePointer&);

		GLuint index_;
		GLuint size_;
		GLuint type_;
		GLuint byteOffset_;
		GLuint stride_;
};
