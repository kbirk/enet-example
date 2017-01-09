#pragma once

#include "Common.h"
#include "input/Window.h"
#include "sdl/SDL2Keyboard.h"
#include "sdl/SDL2Mouse.h"

#include <glm/glm.hpp>
#include <epoxy/gl.h>
#include <SDL2/SDL.h>

#include <string>

class SDL2Window : public Window {

	public:

		typedef std::shared_ptr<SDL2Window> Shared;
		static Shared alloc(const std::string, uint32_t, uint32_t);

		SDL2Window(const std::string, uint32_t, uint32_t);
		~SDL2Window();

		void processEvents();
		void swapBuffers() const;

		bool shouldClose() const;
		glm::ivec2 size() const;

		Keyboard::Shared keyboard() const;
		Mouse::Shared mouse() const;
		//Controller::Shared controller() const;

	private:

		void processEvent(const SDL_Event*) const;

		SDL_Window* window_;
		SDL_GLContext context_;
		bool shouldClose_;

		SDL2Keyboard::Shared keyboard_;
		SDL2Mouse::Shared mouse_;
		//std::map<uint8_t, SDL2Controller::Shared> controllers_;
};
