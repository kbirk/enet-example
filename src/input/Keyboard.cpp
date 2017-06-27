#include "input/Keyboard.h"

Keyboard::Keyboard()
{
}

Keyboard::~Keyboard()
{
}

void Keyboard::add(KeyboardEventHandler handler)
{
    handlers_.push_back(handler);
}

std::vector<Input::Shared> Keyboard::poll()
{
    auto input = input_;
    input_ = std::vector<Input::Shared>();
    return input;
}

void Keyboard::dispatchEvent(
    const KeyboardEvent& event,
    const std::map<Key, KeyState>& keyboardState,
    const std::map<Button, ButtonState>& mouseState)
{
    // dispatch events
    for (auto handler : handlers_) {
        auto res = handler(event, keyboardState, mouseState);
        if (res) {
            input_.push_back(res);
        }
    }
}
