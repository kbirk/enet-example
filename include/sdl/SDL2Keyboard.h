#pragma once

#include "Common.h"
#include "input/Keyboard.h"
#include "input/KeyboardEnum.h"
#include "input/KeyboardEvent.h"

#include <SDL2/SDL.h>

class SDL2Keyboard : public Keyboard {

	friend class SDL2Window;

	public:

		typedef std::shared_ptr<SDL2Keyboard> Shared;
		static Shared alloc();

		SDL2Keyboard();

		const std::map<Key, KeyState>& state() const;

	private:

		KeyEvent translateKeyEvent(const SDL_Event*) const;
		void updateState(const KeyboardEvent&);
		void handleKeyboardEvent(const SDL_Event*, const std::map<Button, ButtonState>&);

		std::map<Key, KeyState> state_;
};
