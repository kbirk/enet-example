#include "input/Keyboard.h"

Keyboard::Keyboard() {
}

Keyboard::~Keyboard() {
}

void Keyboard::add(KeyboardEventHandler handler) {
	handlers_.push_back(handler);
}

std::vector<Input::Shared> Keyboard::poll() {
	auto input = input_;
	input_ = std::vector<Input::Shared>();
	return input;
}

void Keyboard::dispatchEvent(
	const KeyboardEvent& event,
	const std::map<Key, KeyState>& keyboardState,
	const std::map<Button, ButtonState>& mouseState) {
	// dispatch events
	for (auto handler : handlers_) {
		auto res = handler(event, keyboardState, mouseState);
		if (res) {
			input_.push_back(res);
		}
	}
}

// void Keyboard::handleMovement(const KeyboardEvent event, const std::map<KeyType, KeyState>& state) {
//
// 	// only care about movement keys
// 	if (event.key != KeyType::SCAN_W ||
// 		event.key != KeyType::SCAN_A ||
// 		event.key != KeyType::SCAN_S ||
// 		event.key != KeyType::SCAN_D) {
// 		return;
// 	}
//
// 	// get the state of all movement keys
// 	KeyState w = state[KeyType::SCAN_W];
// 	KeyState a = state[KeyType::SCAN_A];
// 	KeyState s = state[KeyType::SCAN_S];
// 	KeyState d = state[KeyType::SCAN_D];
//
// 	if (event.type == KeyEventType::PRESS) {
//
// 		// press
// 		glm::vec3 direction(0, 0, 0);
// 		if (w == KeyState::DOWN) {
// 			direction += glm::vec3(0, 0, -1);
// 		}
// 		if (a == KeyState::DOWN) {
// 			direction += glm::vec3(-1, 0, 0);
// 		}
// 		if (s == KeyState::DOWN) {
// 			direction += glm::vec3(0, 0, 1);
// 		}
// 		if (d == KeyState::DOWN) {
// 			direction += glm::vec3(1, 0, 0);
// 		}
// 		// update movement direction
// 		input_[InputType::PLAYER_MOVE] = MoveDirection::alloc(glm::normalize(direction);
//
// 	} else if (event.type == KeyEventType::RELEASE) {
//
// 		// release
// 		if (w == KeyState::UP &&
// 			a == KeyState::UP &&
// 			s == KeyState::UP &&
// 			d == KeyState::UP) {
// 			// move stop
// 			input_[InputType::PLAYER_MOVE] = MoveStop::alloc();
// 		}
//
// 	}
// }
//
// void Keyboard::handleJump(const KeyboardEvent event, const std::map<KeyType, KeyState>& state) {
//
// 	// only care about jump key
// 	if (event.key != KeyType::SCAN_SPACE) {
// 		return;
// 	}
//
// 	// only care about fresh press
// 	if (event.type == KeyEventType::PRESS) {
// 		input_[InputType::PLAYER_JUMP] = Jump::alloc();
// 	}
// }
//
// void Keyboard::handleEvent(const KeyboardEvent event, const std::map<KeyType, KeyState>& state) {
// 	handleMovement(event, state);
// 	handleJump(event, state);
// }
