#pragma once

#include "Common.h"
#include "input/Mouse.h"
#include "input/MouseEnum.h"
#include "input/MouseEvent.h"

#include <SDL2/SDL.h>

class SDL2Mouse : public Mouse {

	friend class SDL2Window;

	public:

		typedef std::shared_ptr<SDL2Mouse> Shared;
		static Shared alloc();

		SDL2Mouse();

		const std::map<Button, ButtonState>& state() const;

	private:

		ButtonEvent translateButtonEvent(const SDL_Event*) const;
		void updateState(const MouseButtonEvent&);
		void handleMouseButtonEvent(const SDL_Event*, const std::map<Key, KeyState>&);
		void handleMouseMoveEvent(const SDL_Event*, const std::map<Key, KeyState>&);
		void handleMouseScrollEvent(const SDL_Event*, const std::map<Key, KeyState>&);

		std::map<Button, ButtonState> state_;
};
