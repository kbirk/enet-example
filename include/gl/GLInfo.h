#pragma once

#include <epoxy/gl.h>

#include <string>

namespace GLInfo {

	/**
	 * Return the number of bytes in the provided OpenGL Enum.
	 */
	uint32_t sizeOfType(uint32_t enumType);

	/**
	 * Convert an OpenGL error code into a human readable string.
	 */
	std::string errorToString(GLuint error);
}
