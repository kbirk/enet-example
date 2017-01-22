#pragma once

#include "Common.h"
//#include "input/Controller.h"
#include "input/Keyboard.h"
#include "input/Mouse.h"

#include <glm/glm.hpp>

#include <memory>

class Window {

	public:

		typedef std::shared_ptr<Window> Shared;

		Window();
		virtual ~Window();

		virtual void processEvents() = 0;
		virtual void swapBuffers() const = 0;

		virtual bool shouldClose() const = 0;
		virtual glm::ivec2 size() const = 0;
		virtual glm::ivec2 bufferSize() const = 0;

		virtual Keyboard::Shared keyboard() const = 0;
		virtual Mouse::Shared mouse() const = 0;
		//virtual Controller::Shared controller() const = 0;

		virtual std::vector<Input::Shared> poll() const = 0;

	private:

		// prevent copy-construction
		Window(const Window&);
		// prevent assignment
		Window& operator= (const Window&);
};
