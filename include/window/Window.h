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
	CONTROLLER_UP,
	CONTROLLER_DOWN,
	CONTROLLER_ADD,
	CONTROLLER_REMOVE,
	KEY_UP,
	KEY_DOWN,
	MOUSE_LEFT_UP,
	MOUSE_LEFT_DOWN,
	MOUSE_MIDDLE_UP,
	MOUSE_MIDDLE_DOWN,
	MOUSE_RIGHT_UP,
	MOUSE_RIGHT_DOWN,
	MOUSE_MOVE,
	MOUSE_WHEEL,
	RESIZE,
	CLOSE
};

struct WindowEvent {
	WindowEventType type;
	SDL_Event originalEvent;
};

namespace Window {
	void setup();
	void teardown();
	glm::ivec2 size();
	void swapBuffers();
	bool handleEvents();
	void on(WindowEventType, std::function<void(WindowEvent)>);
}
