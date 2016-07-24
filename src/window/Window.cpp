#include "window/Window.h"

namespace Window {

	const std::string GAME_CONTROLLER_DB = "resources/gamecontrollerdb.txt";
	const std::string WINDOW_TITLE = "ENet Example";
	const uint32_t DEFAULT_WINDOW_WIDTH = 1280;
	const uint32_t DEFAULT_WINDOW_HEIGHT = 720;

	SDL_Window* window;
	SDL_GLContext context;
	std::map<WindowEventType, std::vector<WindowEventFunc>> callbacks;

	void on(WindowEventType id, WindowEventFunc func) {
		auto iter = callbacks.find(id);
		if (iter == callbacks.end()) {
			callbacks[id] = std::vector<WindowEventFunc>();
		}
		callbacks[id].push_back(func);
	}

	void executeCallbacks(WindowEventType type, SDL_Event event) {
		auto iter = callbacks.find(type);
		if (iter != callbacks.end()) {
			WindowEvent msg{
				type,
				event
			};
			for (auto func : iter->second) {
				func(msg);
			}
		}
	}

	void setup() {
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
		// SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		// SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
		// create an application window with the following settings:
		window = SDL_CreateWindow(
			WINDOW_TITLE.c_str(), // window title
			SDL_WINDOWPOS_UNDEFINED, // initial x position
			SDL_WINDOWPOS_UNDEFINED, // initial y position
			DEFAULT_WINDOW_WIDTH, // width, in pixels
			DEFAULT_WINDOW_HEIGHT, // height, in pixels
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI); // flags
		// check that the window was successfully made
		if (window == nullptr) {
			// In the event that the window could not be made...
			LOG_ERROR("Could not create window: " << SDL_GetError());
			return;
		}
		// create the OpenGL context
		context = SDL_GL_CreateContext(window);
		// disable relative mouse mode
		SDL_SetRelativeMouseMode(SDL_FALSE);
		// sync buffer swap with monitor's vertical refresh rate
		SDL_GL_SetSwapInterval(1);
	}

	void teardown() {
		SDL_GL_DeleteContext(context);
		// close and destroy the window
		SDL_DestroyWindow(window);
		// clean up
		SDL_Quit();
	}

	void swapBuffers() {
		SDL_GL_SwapWindow(window);
	}

	glm::ivec2 size() {
		int32_t w = 0;
		int32_t h = 0;
		SDL_GL_GetDrawableSize(window, &w, &h);
		return glm::ivec2(w, h);
	}

	bool handleEvents() {
		uint32_t status = 0;
		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			switch (event.type) {

				case SDL_CONTROLLERDEVICEADDED:
					executeCallbacks(WindowEventType::CONTROLLER_ADD, event);
					break;

				case SDL_CONTROLLERDEVICEREMOVED:
					executeCallbacks(WindowEventType::CONTROLLER_REMOVE, event);
					break;

				case SDL_CONTROLLERBUTTONDOWN:
					executeCallbacks(WindowEventType::CONTROLLER_DOWN, event);
					break;

				case SDL_CONTROLLERBUTTONUP:
					executeCallbacks(WindowEventType::CONTROLLER_UP, event);
					break;

				case SDL_CONTROLLERAXISMOTION:
					executeCallbacks(WindowEventType::CONTROLLER_AXIS, event);
					break;

				case SDL_KEYDOWN:
					executeCallbacks(WindowEventType::KEY_DOWN, event);
					break;

				case SDL_KEYUP:
					// if escape is pressed, quit
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						status = 1; // set status to 1 to exit main loop
					}
					executeCallbacks(WindowEventType::KEY_UP, event);
					break;

				case SDL_MOUSEBUTTONDOWN:
					switch (event.button.button) {
						case SDL_BUTTON_LEFT:
							executeCallbacks(WindowEventType::MOUSE_LEFT_DOWN, event);
							break;

						case SDL_BUTTON_RIGHT:
							executeCallbacks(WindowEventType::MOUSE_RIGHT_DOWN, event);
							break;

						case SDL_BUTTON_MIDDLE:
							executeCallbacks(WindowEventType::MOUSE_MIDDLE_DOWN, event);
							break;
					}
					break;

				case SDL_MOUSEBUTTONUP:
					switch (event.button.button) {
						case SDL_BUTTON_LEFT:
							executeCallbacks(WindowEventType::MOUSE_LEFT_UP, event);
							break;

						case SDL_BUTTON_RIGHT:
							executeCallbacks(WindowEventType::MOUSE_RIGHT_UP, event);
							break;

						case SDL_BUTTON_MIDDLE:
							executeCallbacks(WindowEventType::MOUSE_MIDDLE_UP, event);
							break;
					}
					break;

				case SDL_MOUSEWHEEL:
					executeCallbacks(WindowEventType::MOUSE_WHEEL, event);
					break;


				case SDL_MOUSEMOTION:
					executeCallbacks(WindowEventType::MOUSE_MOVE, event);
					break;

				case SDL_QUIT:
					status = 1;
					executeCallbacks(WindowEventType::CLOSE, event);
					break;

				case SDL_WINDOWEVENT:
					switch (event.window.event) {
						case SDL_WINDOWEVENT_RESIZED:
						case SDL_WINDOWEVENT_SIZE_CHANGED:
							executeCallbacks(WindowEventType::RESIZE, event);
							break;
					}
					break;
			}
		}
		return (status == 1);
	}

}
