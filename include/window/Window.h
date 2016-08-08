#pragma once

#include "Common.h"
#include "log/Log.h"

#include <glm/glm.hpp>

#include <epoxy/gl.h>
#include <SDL2/SDL.h>

#include <memory>
#include <string>
#include <map>
#include <vector>

enum class WindowEventType {
	CONTROLLER_AXIS,
	CONTROLLER_PRESS,
	CONTROLLER_RELEASE,
	CONTROLLER_ADD,
	CONTROLLER_REMOVE,
	KEY_PRESS,
	KEY_RELEASE,
	MOUSE_LEFT_PRESS,
	MOUSE_LEFT_RELEASE,
	MOUSE_MIDDLE_PRESS,
	MOUSE_MIDDLE_RELEASE,
	MOUSE_RIGHT_PRESS,
	MOUSE_RIGHT_RELEASE,
	MOUSE_MOVE,
	MOUSE_WHEEL,
	RESIZE,
	CLOSE
};

struct WindowEvent {
	WindowEventType type;
	SDL_Event* originalEvent;
};

typedef std::function<void(WindowEvent&)> WindowEventFunc;

namespace Window {
	void setup();
	void teardown();
	glm::ivec2 size();
	void swapBuffers();
	bool handleEvents();
	const uint8_t* pollKeyboard();
	void on(WindowEventType, WindowEventFunc);
}
