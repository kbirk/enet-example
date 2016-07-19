#include "gl/GLInfo.h"

namespace GLInfo {

	std::string errorToString(GLenum errorCode) {
		switch (errorCode) {
			case GL_NO_ERROR:
				return "No error has been recorded.";
			case GL_INVALID_ENUM:
				return "GL_INVALID_ENUM: An unacceptable value is specified "
					"for an enumerated argument. The offending command is "
					"ignored and has no other side effect than to set the "
					"error flag.";
			case GL_INVALID_VALUE:
				return "GL_INVALID_VALUE: A numeric argument is out of range. "
					"The offending command is ignored and has no other side "
					"effect than to set the error flag.";
			case GL_INVALID_OPERATION:
				return "GL_INVALID_OPERATION: The specified operation is not "
					"allowed in the current state. The offending command is "
					"ignored and has no other side effect than to set the "
					"error flag.";
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer "
					"object is not complete. The offending command is ignored "
					"and has no other side effect than to set the error flag.";
			case GL_OUT_OF_MEMORY:
				return "GL_OUT_OF_MEMORY: There is not enough memory left to "
					"execute the command. The state of the GL is undefined, "
					"except for the state of the error flags, after this error "
					"is recorded.";
			case GL_STACK_UNDERFLOW:
				return "GL_STACK_UNDERFLOW: An attempt has been made to "
					"perform an operation that would cause an internal stack "
					"to underflow.";
			case GL_STACK_OVERFLOW:
				return "GL_STACK_OVERFLOW: An attempt has been made to perform "
					"an operation that would cause an internal stack to "
					"overflow.";
		}
		return "UNRECOGNIZED_ERROR: The error code is unrecognized";
	}

	uint32_t sizeOfType(uint32_t glEnum) {
		switch(glEnum) {
			case GL_UNSIGNED_BYTE:
				return sizeof(GLubyte);
			case GL_BYTE:
				return sizeof(GLbyte);
			case GL_UNSIGNED_SHORT:
				return sizeof(GLushort);
			case GL_SHORT:
				return sizeof(GLshort);
			case GL_UNSIGNED_INT:
				return sizeof(GLuint);
			case GL_INT:
				return sizeof(GLint);
			case GL_FLOAT:
				return sizeof(GLfloat);
			case GL_DOUBLE:
				return sizeof(GLdouble);
		}
		return 0;
	}
}
