#include "sdl/SDL2Keyboard.h"

#include "time/Time.h"

const std::map<SDL_Scancode, Key> KEY_TYPE_MAP = {
	{ SDL_SCANCODE_A, Key::SCAN_A },
	{ SDL_SCANCODE_B, Key::SCAN_B },
	{ SDL_SCANCODE_C, Key::SCAN_C },
	{ SDL_SCANCODE_D, Key::SCAN_D },
	{ SDL_SCANCODE_E, Key::SCAN_E },
	{ SDL_SCANCODE_F, Key::SCAN_F },
	{ SDL_SCANCODE_G, Key::SCAN_G },
	{ SDL_SCANCODE_H, Key::SCAN_H },
	{ SDL_SCANCODE_I, Key::SCAN_I },
	{ SDL_SCANCODE_J, Key::SCAN_J },
	{ SDL_SCANCODE_K, Key::SCAN_K },
	{ SDL_SCANCODE_L, Key::SCAN_L },
	{ SDL_SCANCODE_M, Key::SCAN_M },
	{ SDL_SCANCODE_N, Key::SCAN_N },
	{ SDL_SCANCODE_O, Key::SCAN_O },
	{ SDL_SCANCODE_P, Key::SCAN_P },
	{ SDL_SCANCODE_Q, Key::SCAN_Q },
	{ SDL_SCANCODE_R, Key::SCAN_R },
	{ SDL_SCANCODE_S, Key::SCAN_S },
	{ SDL_SCANCODE_T, Key::SCAN_T },
	{ SDL_SCANCODE_U, Key::SCAN_U },
	{ SDL_SCANCODE_V, Key::SCAN_V },
	{ SDL_SCANCODE_W, Key::SCAN_W },
	{ SDL_SCANCODE_X, Key::SCAN_X },
	{ SDL_SCANCODE_Y, Key::SCAN_Y },
	{ SDL_SCANCODE_Z, Key::SCAN_Z },
	{ SDL_SCANCODE_0, Key::SCAN_0 },
	{ SDL_SCANCODE_1, Key::SCAN_1 },
	{ SDL_SCANCODE_2, Key::SCAN_2 },
	{ SDL_SCANCODE_3, Key::SCAN_3 },
	{ SDL_SCANCODE_4, Key::SCAN_4 },
	{ SDL_SCANCODE_5, Key::SCAN_5 },
	{ SDL_SCANCODE_6, Key::SCAN_6 },
	{ SDL_SCANCODE_7, Key::SCAN_7 },
	{ SDL_SCANCODE_8, Key::SCAN_8 },
	{ SDL_SCANCODE_9, Key::SCAN_9 },
	{ SDL_SCANCODE_F1, Key::SCAN_F1 },
	{ SDL_SCANCODE_F2, Key::SCAN_F2 },
	{ SDL_SCANCODE_F3, Key::SCAN_F3 },
	{ SDL_SCANCODE_F4, Key::SCAN_F4 },
	{ SDL_SCANCODE_F5, Key::SCAN_F5 },
	{ SDL_SCANCODE_F6, Key::SCAN_F6 },
	{ SDL_SCANCODE_F7, Key::SCAN_F7 },
	{ SDL_SCANCODE_F8, Key::SCAN_F8 },
	{ SDL_SCANCODE_F9, Key::SCAN_F9 },
	{ SDL_SCANCODE_F10, Key::SCAN_F10 },
	{ SDL_SCANCODE_F11, Key::SCAN_F11 },
	{ SDL_SCANCODE_F12, Key::SCAN_F12 },
	{ SDL_SCANCODE_SPACE, Key::SCAN_SPACE }
};

SDL2Keyboard::Shared SDL2Keyboard::alloc() {
	return std::make_shared<SDL2Keyboard>();
}

SDL2Keyboard::SDL2Keyboard() {
	for (auto iter : KEY_TYPE_MAP) {
		auto key = iter.second;
		state_[key] = KeyState::UP;
	}
}

const std::map<Key, KeyState>& SDL2Keyboard::state() const {
	return state_;
}

KeyEvent SDL2Keyboard::translateKeyEvent(const SDL_Event* event) const {
	if (event->key.type == SDL_KEYDOWN) {
		if (event->key.repeat != 0) {
			return KeyEvent::HOLD;
		} else {
			return KeyEvent::PRESS;
		}
	}
	return KeyEvent::RELEASE;
}

void SDL2Keyboard::updateState(const KeyboardEvent& event) {
	if (event.type == KeyEvent::PRESS) {
		state_[event.key] = KeyState::DOWN;
	}
	if (event.type == KeyEvent::RELEASE) {
		state_[event.key] = KeyState::UP;
	}
}

void SDL2Keyboard::handleKeyboardEvent(const SDL_Event* sdl, const std::map<Button, ButtonState>& state) {
	auto iter = KEY_TYPE_MAP.find(sdl->key.keysym.scancode);
	if (iter != KEY_TYPE_MAP.end()) {
		// get event
		auto key = iter->second;
		auto type = translateKeyEvent(sdl);
		auto timestamp = Time::fromMilliseconds(sdl->key.timestamp);
		auto event = KeyboardEvent(type, key, timestamp);
		// update state
		updateState(event);
		// dispatch event / state
		dispatchEvent(event, state_, state);
	}
}
