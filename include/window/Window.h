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
	MOUSE_PRESS,
	MOUSE_RELEASE,
	MOUSE_MOVE,
	MOUSE_WHEEL,
	RESIZE,
	CLOSE
};

struct WindowEvent {
	WindowEventType type;
	SDL_Event* originalEvent;
};

typedef std::function<void(const WindowEvent&)> WindowEventFunc;

namespace Window {
	void setup();
	void teardown();
	glm::ivec2 size();
	void swapBuffers();
	void handleEvents();
	const uint8_t* pollKeyboard();
	void on(WindowEventType, WindowEventFunc);
}
