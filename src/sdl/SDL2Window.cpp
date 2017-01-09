#include "sdl/SDL2Window.h"

#include "log/Log.h"

SDL2Window::Shared SDL2Window::alloc(const std::string title, uint32_t width, uint32_t height) {
	return std::make_shared<SDL2Window>(title, width, height);
}

SDL2Window::SDL2Window(const std::string title, uint32_t width, uint32_t height)
	: shouldClose_(false) {
	// initialize SDL2
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
	// set the opengl context version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// set byte depths
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// muli-sampling
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	// create an application window with the following settings:
	window_ = SDL_CreateWindow(
		title.c_str(), // window title
		SDL_WINDOWPOS_UNDEFINED, // initial x position
		SDL_WINDOWPOS_UNDEFINED, // initial y position
		width, // width, in pixels
		height, // height, in pixels
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN /*| SDL_WINDOW_RESIZABLE*/ | SDL_WINDOW_ALLOW_HIGHDPI); // flags
	// check that the window was successfully made
	if (window_ == nullptr) {
		// In the event that the window could not be made...
		LOG_ERROR("Could not create window: " << SDL_GetError());
		return;
	}
	// create the OpenGL context
	context_ = SDL_GL_CreateContext(window_);
	// disable relative mouse mode
	SDL_SetRelativeMouseMode(SDL_FALSE);
	// sync buffer swap with monitor's vertical refresh rate
	SDL_GL_SetSwapInterval(1);

	// allocation keyboard / mouse
	keyboard_ = SDL2Keyboard::alloc();
	mouse_ = SDL2Mouse::alloc();
}

SDL2Window::~SDL2Window() {
	SDL_GL_DeleteContext(context_);
	// close and destroy the window
	SDL_DestroyWindow(window_);
	// clean up
	SDL_Quit();
}

bool SDL2Window::shouldClose() const {
	return shouldClose_;
}

void SDL2Window::swapBuffers() const{
	SDL_GL_SwapWindow(window_);
}

glm::ivec2 SDL2Window::size() const {
	int32_t w = 0;
	int32_t h = 0;
	SDL_GL_GetDrawableSize(window_, &w, &h);
	return glm::ivec2(w, h);
}

void SDL2Window::processEvent(const SDL_Event* event) const {

	switch (event->type) {

		case SDL_CONTROLLERDEVICEADDED:
		case SDL_CONTROLLERDEVICEREMOVED:
		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERBUTTONUP:
		case SDL_CONTROLLERAXISMOTION:
			// TODO: impl controller
			break;

		case SDL_KEYDOWN:
		case SDL_KEYUP:
			keyboard_->handleKeyboardEvent(event, mouse_->state());
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			mouse_->handleMouseButtonEvent(event, keyboard_->state());
			break;

		case SDL_MOUSEWHEEL:
			mouse_->handleMouseScrollEvent(event, keyboard_->state());
			break;

		case SDL_MOUSEMOTION:
			mouse_->handleMouseMoveEvent(event, keyboard_->state());
			break;
	}
}

void SDL2Window::processEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {

		switch (event.type) {

			// TODO: add logic for tracking controllers being added / removed

			case SDL_QUIT:
				shouldClose_ = true;
				break;

			case SDL_WINDOWEVENT:
				switch (event.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
					case SDL_WINDOWEVENT_SIZE_CHANGED:
						// TODO: handle this
						break;
				}
				break;

			default:
				processEvent(&event);
				break;
		}
	}
}

Keyboard::Shared SDL2Window::keyboard() const {
	return keyboard_;
}
Mouse::Shared SDL2Window::mouse() const {
	return mouse_;
}
