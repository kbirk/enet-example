#include "input/MouseEvent.h"

MouseMoveEvent::MouseMoveEvent(glm::vec2 position, glm::vec2 delta, std::time_t timestamp)
	: position(position)
	, delta(delta)
	, timestamp(timestamp) {}

MouseScrollEvent::MouseScrollEvent(float32_t delta, std::time_t timestamp)
	: delta(delta)
	, timestamp(timestamp) {}

MouseButtonEvent::MouseButtonEvent(ButtonEvent type, Button button, glm::vec2 position, std::time_t timestamp)
	: position(position)
	, type(type)
	, button(button)
	, timestamp(timestamp) {}
