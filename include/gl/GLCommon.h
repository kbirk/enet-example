#pragma once

#include "Common.h"
#include "log/Log.h"
#include "gl/GLInfo.h"

#include <epoxy/gl.h>

#include <string>

/**
 * Enable OpenGL logging by default
 */
#define LOGGING_ENABLED 1

/**
 * OpenGL logging macro
 */
#ifdef LOGGING_ENABLED
	#define LOG_OPENGL(x) \
	{ \
		GLuint error = glGetError(); \
		if (error != GL_NO_ERROR) { \
			LOG_ERROR(x << " " << GLInfo::errorToString(error)); \
		} \
	}
#else
	#define LOG_OPENGL(x)
#endif
