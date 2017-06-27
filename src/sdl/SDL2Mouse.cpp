#include "sdl/SDL2Mouse.h"

#include "time/Time.h"

const std::map<uint8_t, Button> BUTTON_TYPE_MAP = {
    { SDL_BUTTON_LEFT, Button::LEFT },
    { SDL_BUTTON_MIDDLE, Button::MIDDLE },
    { SDL_BUTTON_RIGHT, Button::RIGHT }
};

SDL2Mouse::Shared SDL2Mouse::alloc()
{
    return std::make_shared<SDL2Mouse>();
}

SDL2Mouse::SDL2Mouse()
{
    for (auto iter : BUTTON_TYPE_MAP) {
        auto key = iter.second;
        state_[key] = ButtonState::UP;
    }
}

const std::map<Button, ButtonState>& SDL2Mouse::state() const
{
    return state_;
}

ButtonEvent SDL2Mouse::translateButtonEvent(const SDL_Event* event) const
{
    if (event->button.type == SDL_MOUSEBUTTONDOWN) {
        if (event->button.clicks == 2) {
            return ButtonEvent::DBL_CLICK;
        } else {
            return ButtonEvent::CLICK;
        }
    }
    return ButtonEvent::RELEASE;
}

void SDL2Mouse::updateState(const MouseButtonEvent& event)
{
    if (event.type == ButtonEvent::CLICK || event.type == ButtonEvent::DBL_CLICK) {
        state_[event.button] = ButtonState::DOWN;
    }
    if (event.type == ButtonEvent::RELEASE) {
        state_[event.button] = ButtonState::UP;
    }
}

void SDL2Mouse::handleMouseButtonEvent(const SDL_Event* sdl, const std::map<Key, KeyState>& state)
{
    auto iter = BUTTON_TYPE_MAP.find(sdl->button.button);
    if (iter != BUTTON_TYPE_MAP.end()) {
        // get event
        auto button = iter->second;
        auto type = translateButtonEvent(sdl);
        auto x = sdl->button.x;
        auto y = sdl->button.y;
        auto timestamp = Time::fromMilliseconds(sdl->button.timestamp);
        auto event = MouseButtonEvent(type, button, glm::vec2(x, y), timestamp);
        // update state
        updateState(event);
        // dispatch event / state
        dispatchEvent(event, state_, state);
    }
}

void SDL2Mouse::handleMouseMoveEvent(const SDL_Event* sdl, const std::map<Key, KeyState>& state)
{
    // get event
    auto dx = sdl->motion.xrel;
    auto dy = sdl->motion.yrel;
    auto x = sdl->motion.x;
    auto y = sdl->motion.y;
    auto timestamp = Time::fromMilliseconds(sdl->motion.timestamp);
    auto event = MouseMoveEvent(glm::vec2(x, y), glm::vec2(dx, dy), timestamp);
    // dispatch event / state
    dispatchEvent(event, state_, state);
}

void SDL2Mouse::handleMouseScrollEvent(const SDL_Event* sdl, const std::map<Key, KeyState>& state)
{
    // get event
    auto delta = sdl->wheel.y;
    auto timestamp = Time::fromMilliseconds(sdl->wheel.timestamp);
    auto event = MouseScrollEvent(delta, timestamp);
    // dispatch event / state
    dispatchEvent(event, state_, state);
}
