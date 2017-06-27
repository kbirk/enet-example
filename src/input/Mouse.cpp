#include "input/Mouse.h"

Mouse::Mouse()
{
}

Mouse::~Mouse()
{
}

void Mouse::add(MouseMoveHandler handler)
{
    moveHandlers_.push_back(handler);
}

void Mouse::add(MouseButtonHandler handler)
{
    buttonHandlers_.push_back(handler);
}

void Mouse::add(MouseScrollHandler handler)
{
    scrollHandlers_.push_back(handler);
}

std::vector<Input::Shared> Mouse::poll()
{
    auto input = input_;
    input_ = std::vector<Input::Shared>();
    return input;
}

void Mouse::dispatchEvent(
    const MouseMoveEvent& event,
    const std::map<Button, ButtonState>& mouseState,
    const std::map<Key, KeyState>& keyboardState)
{
    // dispatch events
    for (auto handler : moveHandlers_) {
        auto res = handler(event, mouseState, keyboardState);
        if (res) {
            input_.push_back(res);
        }
    }
}

void Mouse::dispatchEvent(
    const MouseButtonEvent& event,
    const std::map<Button, ButtonState>& mouseState,
    const std::map<Key, KeyState>& keyboardState)
{
    // dispatch events
    for (auto handler : buttonHandlers_) {
        auto res = handler(event, mouseState, keyboardState);
        if (res) {
            input_.push_back(res);
        }
    }
}

void Mouse::dispatchEvent(
    const MouseScrollEvent& event,
    const std::map<Button, ButtonState>& mouseState,
    const std::map<Key, KeyState>& keyboardState)
{
    // dispatch events
    for (auto handler : scrollHandlers_) {
        auto res = handler(event, mouseState, keyboardState);
        if (res) {
            input_.push_back(res);
        }
    }
}

// const float32_t DEFAULT_DISTANCE = 10.0f;
// const float32_t SCROLL_FACTOR = 0.5f;
// const float32_t MAX_DISTANCE = DEFAULT_DISTANCE * 10.0;
// const float32_t MIN_DISTANCE = 0.0;
// const float32_t X_FACTOR = -0.1;
// const float32_t Y_FACTOR = -0.2;
//
// void Mouse::handleMove(const MouseEvent& event, const std::map<Button, ButtonState>& state) {
// 	if (state[Button::LEFT]) {
// 		float32_t xAngle = event.delta.x * X_FACTOR * (M_PI / 180.0);
// 		float32_t yAngle = event.delta.y * Y_FACTOR * (M_PI / 180.0);
// 		// camera->setTranslation(glm::vec3(0, 0, 0));
// 		// camera->rotateGlobal(xAngle, glm::vec3(0, 1, 0));
// 		// camera->rotateLocal(yAngle, glm::vec3(1, 0, 0));
// 		// camera->translateLocal(glm::vec3(0, 0, distance));
// 		input_[InputType::CAMERA_ROTATE] = CameraRotate::alloc(xAngle, yAngle);
// 	}
// }
//
// void Mouse::handleScroll(const MouseEvent& event, const std::map<KeyType, KeyState>& state) {
// 	float32_t delta = (event.delta * -SCROLL_FACTOR);
// 	// distance += delta;
// 	// distance = std::min(std::max(distance, MIN_DISTANCE), MAX_DISTANCE);
// 	// camera->setTranslation(glm::vec3(0, 0, 0));
// 	// camera->translateLocal(glm::vec3(0, 0, distance));
// 	input_[InputType::CAMERA_ZOOM] = CameraZoom::alloc(delta);
// }
//
// void Mouse::handleEvent(const MouseEvent& event, const std::map<KeyType, KeyState>& state) {
// 	handleMove(event, state);
// 	handleScroll(event, state);
// }
