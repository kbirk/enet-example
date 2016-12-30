#include "Common.h"
#include "game/Common.h"
#include "game/Frame.h"
#include "geometry/Cube.h"
#include "gl/GLCommon.h"
#include "gl/ElementArrayBufferObject.h"
#include "gl/VertexFragmentShader.h"
#include "gl/VertexArrayObject.h"
#include "gl/VertexAttributePointer.h"
#include "gl/VertexBufferObject.h"
#include "gl/Uniform.h"
#include "gl/UniformType.h"
#include "log/Log.h"
#include "math/Transform.h"
#include "net/DeliveryType.h"
#include "net/Client.h"
#include "net/Message.h"
#include "render/RenderCommand.h"
#include "render/Renderer.h"
#include "serial/StreamBuffer.h"
#include "time/Time.h"
#include "window/Window.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <algorithm>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>
#include <deque>

const std::string HOST = "localhost";
const uint32_t PORT = 7000;
const std::time_t DISCONNECT_TIMEOUT = Time::seconds(5);

const float32_t DEFAULT_DISTANCE = 10.0f;
const float32_t SCROLL_FACTOR = 0.005f;
const float32_t MAX_DISTANCE = DEFAULT_DISTANCE * 10.0;
const float32_t MIN_DISTANCE = 0.0;
const float32_t X_FACTOR = -0.1;
const float32_t Y_FACTOR = -0.2;

const float32_t FIELD_OF_VIEW = 60.0f / (180.0f / M_PI);
const float32_t NEAR_PLANE = 0.1f;
const float32_t FAR_PLANE = 10000.0f;

bool down = false;
float32_t distance = DEFAULT_DISTANCE;

bool quit = false;

VertexFragmentShader::Shared flatShader;
VertexFragmentShader::Shared phongShader;
VertexArrayObject::Shared cube;
VertexArrayObject::Shared x;
VertexArrayObject::Shared y;
VertexArrayObject::Shared z;
Viewport::Shared viewport;
glm::mat4 projection;
Client::Shared client;
Transform::Shared camera;

std::deque<Frame::Shared> frames;

void add_frame(Frame::Shared frame) {
	// prepend
	frames.push_front(frame);
}

std::string shader_path(const std::string& str, const std::string& type) {
	return "resources/shaders/" + str + "." + type;
}

void load_shaders() {
	// flat
	flatShader = VertexFragmentShader::alloc();
	flatShader->create(shader_path("flat", "vert"), shader_path("flat", "frag"));
	// phong
	phongShader = VertexFragmentShader::alloc();
	phongShader->create(shader_path("phong", "vert"), shader_path("phong", "frag"));
}

void load_cube() {
	// positions
	auto positions = VertexBufferObject::alloc();
	positions->upload(Cube::positions());
	// normals
	auto normals = VertexBufferObject::alloc();
	normals->upload(Cube::normals());
	// indices
	auto indices = ElementArrayBufferObject::alloc();
	indices->upload(Cube::indices());
	// vao
	cube = VertexArrayObject::alloc();
	cube->attach(positions, VertexAttributePointer::alloc(0, 3, GL_FLOAT));
	cube->attach(normals, VertexAttributePointer::alloc(1, 3, GL_FLOAT));
	cube->attach(indices);
	cube->upload();
}

VertexArrayObject::Shared load_axis(const glm::vec3& axis) {
	// positions
	auto positions = VertexBufferObject::alloc();
	positions->upload(std::vector<float32_t> {
		0.0f, 0.0f, 0.0f,
		axis.x, axis.y, axis.z
	});
	// indices
	auto indices = ElementArrayBufferObject::alloc(
		GL_UNSIGNED_INT,
		GL_LINES);
	indices->upload(std::vector<uint32_t>{
		0, 1
	});
	auto vao = VertexArrayObject::alloc();
	vao->attach(positions, VertexAttributePointer::alloc(0, 3, GL_FLOAT));
	vao->attach(indices);
	vao->upload();
	return vao;
}

void load_axes() {
	x = load_axis(glm::vec3(1, 0, 0));
	y = load_axis(glm::vec3(0, 1, 0));
	z = load_axis(glm::vec3(0, 0, 1));
}

RenderCommand::Shared render_phong(VertexArrayObject::Shared vao, glm::mat4 model) {
	auto command = RenderCommand::alloc();
	command->uniforms[UniformType::MODEL_MATRIX] = Uniform::alloc(model);
	command->uniforms[UniformType::VIEW_MATRIX] = Uniform::alloc(camera->viewMatrix());
	command->uniforms[UniformType::PROJECTION_MATRIX] = Uniform::alloc(projection);
	command->uniforms[UniformType::LIGHT_POSITION0] = Uniform::alloc(glm::vec3(0.0, 10.0, 10.0));
	command->uniforms[UniformType::SPECULAR_COLOR] = Uniform::alloc(glm::vec4(1.0, 1.0, 1.0, 1.0));
	command->uniforms[UniformType::DIFFUSE_COLOR] = Uniform::alloc(glm::vec4(0.5, 0.5, 0.5, 1.0));
	command->uniforms[UniformType::AMBIENT_COLOR] = Uniform::alloc(glm::vec4(0.2, 0.2, 0.2, 1.0));
	command->uniforms[UniformType::SHININESS] = Uniform::alloc(10.0f);
	command->enables.push_back(GL_DEPTH_TEST);
	command->enables.push_back(GL_BLEND);
	command->viewport = viewport;
	command->shader = phongShader;
	command->vao = vao;
	return command;
}

RenderCommand::Shared render_flat(VertexArrayObject::Shared vao, glm::mat4 model) {
	auto command = RenderCommand::alloc();
	command->uniforms[UniformType::MODEL_MATRIX] = Uniform::alloc(model);
	command->uniforms[UniformType::VIEW_MATRIX] = Uniform::alloc(camera->viewMatrix());
	command->uniforms[UniformType::PROJECTION_MATRIX] = Uniform::alloc(projection);
	command->uniforms[UniformType::SPECULAR_COLOR] = Uniform::alloc(glm::vec4(1.0, 1.0, 1.0, 1.0));
	command->uniforms[UniformType::DIFFUSE_COLOR] = Uniform::alloc(glm::vec4(0.5, 0.5, 0.5, 1.0));
	command->uniforms[UniformType::AMBIENT_COLOR] = Uniform::alloc(glm::vec4(0.2, 0.2, 0.2, 1.0));
	command->uniforms[UniformType::SHININESS] = Uniform::alloc(10.0f);
	command->enables.push_back(GL_DEPTH_TEST);
	command->enables.push_back(GL_BLEND);
	command->viewport = viewport;
	command->shader = flatShader;
	command->vao = vao;
	return command;
}

RenderCommand::Shared render_axis(const VertexArrayObject::Shared& vao, const glm::vec3& color) {
	auto command = RenderCommand::alloc();
	command->uniforms[UniformType::MODEL_MATRIX] = Uniform::alloc(glm::mat4(1.0));
	command->uniforms[UniformType::VIEW_MATRIX] = Uniform::alloc(camera->viewMatrix());
	command->uniforms[UniformType::PROJECTION_MATRIX] = Uniform::alloc(projection);
	command->uniforms[UniformType::DIFFUSE_COLOR] = Uniform::alloc(color);
	command->enables.push_back(GL_DEPTH_TEST);
	command->shader = flatShader;
	command->vao = vao;
	return command;
}

std::vector<RenderCommand::Shared> render_axes() {
	auto commands = std::vector<RenderCommand::Shared>();
	commands.push_back(render_axis(x, glm::vec3(1, 0, 0)));
	commands.push_back(render_axis(y, glm::vec3(0, 1, 0)));
	commands.push_back(render_axis(z, glm::vec3(0, 0, 1)));
	return commands;
}

void signal_handler(int32_t signal) {
	LOG_DEBUG("Caught signal: " << signal << ", shutting down...");
	quit = true;
}

void handle_disconnect() {
	// sleep
	Time::sleep(DISCONNECT_TIMEOUT);
	// attempt to reconnect
	while (!quit) {
		LOG_DEBUG("Attempting to re-connect to server...");
		if (!client->connect(HOST, PORT)) {
			// success
			break;
		}
	}
}

void update_view() {
	auto size = Window::size();
	// update viewport size
	viewport->resize(0, 0, size.x, size.y);
	// update projection
	projection = glm::perspective(FIELD_OF_VIEW, float32_t(size.x) / float32_t(size.y), NEAR_PLANE, FAR_PLANE);
}

void handle_mouse_press(const WindowEvent& event) {
	if (event.originalEvent->button.button == SDL_BUTTON_LEFT) {
		down = true;
	}
}

void handle_mouse_release(const WindowEvent& event) {
	if (event.originalEvent->button.button == SDL_BUTTON_LEFT) {
		down = false;
	}
}

void handle_mouse_move(const WindowEvent& event) {
	int32_t dx = event.originalEvent->motion.xrel;
	int32_t dy = event.originalEvent->motion.yrel;
	if (down) {
		float32_t xAngle = dx * X_FACTOR * (M_PI / 180.0);
		float32_t yAngle = dy * Y_FACTOR * (M_PI / 180.0);
		camera->setTranslation(glm::vec3(0, 0, 0));
		camera->rotateGlobal(xAngle, glm::vec3(0, 1, 0));
		camera->rotateLocal(yAngle, glm::vec3(1, 0, 0));
		camera->translateLocal(glm::vec3(0, 0, distance));
	}
}

void handle_mouse_wheel(const WindowEvent& event) {
	float32_t y = event.originalEvent->wheel.y;
	distance += (y * -SCROLL_FACTOR * MAX_DISTANCE);
	distance = std::min(std::max(distance, MIN_DISTANCE), MAX_DISTANCE);
	camera->setTranslation(glm::vec3(0, 0, 0));
	camera->translateLocal(glm::vec3(0, 0, distance));
}

void handle_key_press(const WindowEvent& event) {
	if (event.originalEvent->key.keysym.sym == SDLK_ESCAPE) {
		quit = true;
	}
}

void handle_close(const WindowEvent& event) {
	quit = true;
}

void handle_keyboard(const uint8_t* states) {

	float32_t SPEED = 0.1;
	glm::vec3 direction(0, 0, 0);

	if (states[SDL_SCANCODE_W]) {
		direction += glm::vec3(0, 0, -1);
	}
	if (states[SDL_SCANCODE_A]) {
		direction += glm::vec3(-1, 0, 0);
	}
	if (states[SDL_SCANCODE_S]) {
		direction += glm::vec3(0, 0, 1);
	}
	if (states[SDL_SCANCODE_D]) {
		direction += glm::vec3(1, 0, 0);
	}

	if (glm::length(direction) == 0) {
		return;
	}

	direction = glm::normalize(direction) * SPEED;

	// serialize command
	auto stream = StreamBuffer::alloc();
	stream << direction;
 	auto bytes = stream->buffer();

	// send command
	client->send(DeliveryType::RELIABLE, bytes);
}

void deserialize_frame(StreamBuffer::Shared stream) {
	auto frame = Frame::alloc();
	stream >> frame;
	add_frame(frame);
	return;
}

std::tuple<Frame::Shared, Frame::Shared, float32_t> get_frames(std::time_t now) {
	//
	//  find closest frames on either side of the delayed time such that:
	//
	//      frame0->timestamp() < (now - delay) < frame0->timestamp()
	//
	//              interpolation window
	//                      |
	//                      V
	//                 -----------
	//                 |         |
	//  - - - - - - - - - - - - - - - - - - - - - - - - -
	//  |       |       |   x   |       |   x   |       |
	//
	//  0       1       2       3       4       5       6    <- frames
	//                 |         |
	//                 -----------
	//                      ^               ^
	//                      |               |
	//                   delayed          actual
	//                    time             time
	//                      -----------------
	//                              ^
	//                              |
	//                     interpolation delay

	// we need at least 2 frames
	if (frames.size() < 2) {
		return std::make_tuple(nullptr, nullptr, 0);
	}

	// delay timestamp for interpolation "window"
	auto delayed = now - Game::INTERPOLATION_DELAY;

	// find closest frames on each side of the current time

	// BEST CASE: we have no loss / delay and a window of one frame size
	// AVERAGE CASE: we have a bit of loss / delay and a larger window
	// WORST CASE: there are none "ahead" and we are working with stale data

	// find the closest frame to interpolate from
	// BASE CASE: it's the oldest frame in the buffer
	int32_t fromIndex = frames.size() - 1;
	for (auto i = 0; i < frames.size(); i++) {
		auto frame = frames[i];
		if (delayed - frame->timestamp() > 0) {
			fromIndex = i;
			break;
		}
	}

	// find the closest frame to interpolate to
	// BASE CASE: it's the newest frame in the buffer
	int32_t toIndex = 0;
	for (auto i = int32_t(frames.size()) - 1; i >= 0; i--) {
		auto frame = frames[i];
		if ((delayed - frame->timestamp()) <= 0) {
			toIndex = i;
			break;
		}
	}

	// cases where window is outside of the interpolation buffer
	if (toIndex == fromIndex) {
		if (toIndex == 0 && fromIndex == 0) {
			// we only have stale data
			LOG_INFO("stale data... worst case, " << toIndex << ", num frames: " << frames.size());
			return std::make_tuple(frames[0], frames[0], 1);
		}
		if (toIndex == frames.size() - 1 && fromIndex == frames.size() - 1) {
			// not enough time has passed, not ready to interpolate
			return std::make_tuple(nullptr, nullptr, 0);
		}
	}

	// remove any frames that have expired
	if (frames.size() != fromIndex+1) {
		frames.erase(frames.begin() + (fromIndex+1), frames.end());
	}

	// calculate t value based on what frames are used
	auto from = frames[fromIndex];
	auto to = frames[toIndex];

	// calc t value
	float32_t t = float32_t(delayed - from->timestamp()) / float32_t(to->timestamp() - from->timestamp());

	// LOG_INFO("From " << from->timestamp() << " to " << to->timestamp() << ", delayed: " << delayed << ", t: " << t);

	return std::make_tuple(from, to, t);
}

void process_frame(std::time_t now) {

	// update viewport and projection
	update_view();

	// handle events
	Window::handleEvents();

	// poll keyboard state
	auto states = Window::pollKeyboard();
	handle_keyboard(states);

	Frame::Shared a, b;
	float32_t t;

	std::tie(a, b, t) = get_frames(now);

	if (!a || !b) {
		return;
	}

	// interpolate frame
	auto frame = interpolate(a, b, t);

	// clear buffers
	glClearColor(0.137f, 0.137f, 0.137f, 1.0f);
	LOG_OPENGL("glClearColor");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	LOG_OPENGL("glClear");

	// draw origin
	Renderer::render(render_axes());

	// draw players
	for (auto iter : frame->players()) {
		auto player = iter.second;
		Renderer::render(render_phong(cube, player->matrix()));
	}

	// swap back buffer
	Window::swapBuffers();
}

void load_viewport() {
	auto size = Window::size();
	viewport = Viewport::alloc(0, 0, size.x, size.y);
	camera = Transform::alloc();
	camera->setTranslation(glm::vec3(0, 0.0, DEFAULT_DISTANCE));
}

int main(int argc, char** argv) {

	std::srand(std::time(0));

	std::signal(SIGINT, signal_handler);
	std::signal(SIGQUIT, signal_handler);
	std::signal(SIGTERM, signal_handler);

	Window::setup();

	Window::on(WindowEventType::MOUSE_PRESS, handle_mouse_press);
	Window::on(WindowEventType::MOUSE_RELEASE, handle_mouse_release);
	Window::on(WindowEventType::MOUSE_MOVE, handle_mouse_move);
	Window::on(WindowEventType::MOUSE_WHEEL, handle_mouse_wheel);
	Window::on(WindowEventType::KEY_PRESS, handle_key_press);
	Window::on(WindowEventType::CLOSE, handle_close);

	load_viewport();
	load_cube();
	load_shaders();
	load_axes();

	client = Client::alloc();

	if (client->connect(HOST, PORT)) {
		return 1;
	}

	while (true) {

		std::time_t now = Time::timestamp();

		// process the frame
		process_frame(now);

		// poll for events
		auto messages = client->poll();

		// process events
		for (auto msg : messages) {
			if (msg->type() == MessageType::DISCONNECT) {
				// handle the disconnect
				handle_disconnect();
				// ignore other messages
				break;
			} else if (msg->type() == MessageType::DATA) {
				// handle message
				auto stream = msg->stream();
				deserialize_frame(stream);
			}
		}

		// check if exit
		if (quit) {
			break;
		}
	}

	// attempt to disconnect gracefully
	client->disconnect();

	Window::teardown();
}
