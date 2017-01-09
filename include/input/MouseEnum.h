#pragma once

#include "Common.h"

enum class ButtonEvent {
	CLICK,
	DBL_CLICK,
	RELEASE,
};

enum class ButtonState {
	DOWN,
	UP
};

enum class Button {
	LEFT,
	MIDDLE,
	RIGHT
};
