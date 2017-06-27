#pragma once

#include "Common.h"
#include "input/MouseEnum.h"

#include <glm/glm.hpp>

struct MouseMoveEvent {

    MouseMoveEvent(glm::vec2, glm::vec2, std::time_t);

    glm::vec2 position;
    glm::vec2 delta;
    std::time_t timestamp;
};

struct MouseScrollEvent {

    MouseScrollEvent(float32_t, std::time_t);

    float32_t delta;
    std::time_t timestamp;
};

struct MouseButtonEvent {

    MouseButtonEvent(ButtonEvent, Button, glm::vec2 position, std::time_t);

    glm::vec2 position;
    ButtonEvent type;
    Button button;
    std::time_t timestamp;
};
