#include "Common.h"
#include "game/Common.h"
#include "game/Frame.h"
#include "game/InputType.h"
#include "game/Terrain.h"
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
#include "sdl/SDL2Window.h"
#include "serial/StreamBuffer.h"
#include "time/Time.h"

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
const float32_t SCROLL_FACTOR = 0.5f;
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

Window::Shared window;
Keyboard::Shared keyboard;
Mouse::Shared mouse;

VertexFragmentShader::Shared flatShader;
VertexFragmentShader::Shared phongShader;
VertexFragmentShader::Shared terrainShader;
VertexArrayObject::Shared cube;
VertexArrayObject::Shared x;
VertexArrayObject::Shared y;
VertexArrayObject::Shared z;
Viewport::Shared viewport;

glm::mat4 projection;

Client::Shared client;
Transform::Shared camera;

std::deque<Frame::Shared> frames;
Terrain::Shared terrain;

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
	// terrain
	terrainShader = VertexFragmentShader::alloc();
	terrainShader->create(shader_path("terrain", "vert"), shader_path("terrain", "frag"));
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

std::vector<RenderCommand::Shared> render_terrain(Terrain::Shared terrain) {
	auto commands = std::vector<RenderCommand::Shared>();
	auto command = RenderCommand::alloc();
	command->uniforms[UniformType::MODEL_MATRIX] = Uniform::alloc(terrain->transform()->matrix());
	command->uniforms[UniformType::VIEW_MATRIX] = Uniform::alloc(camera->viewMatrix());
	command->uniforms[UniformType::PROJECTION_MATRIX] = Uniform::alloc(projection);
	command->uniforms[UniformType::LIGHT_POSITION0] = Uniform::alloc(glm::vec3(0.0, 10.0, 10.0));
	command->uniforms[UniformType::TEXTURE_SAMPLER0] = Uniform::alloc(0);
	command->uniforms[UniformType::TEXTURE_SAMPLER1] = Uniform::alloc(1);
	command->uniforms[UniformType::TEXTURE_SAMPLER2] = Uniform::alloc(2);
	command->uniforms[UniformType::TEXTURE_SAMPLER3] = Uniform::alloc(3);
	command->textures[GL_TEXTURE0] = terrain->texture(0);
	command->textures[GL_TEXTURE1] = terrain->texture(1);
	command->textures[GL_TEXTURE2] = terrain->texture(2);
	command->textures[GL_TEXTURE3] = terrain->texture(3);
	command->enables.push_back(GL_DEPTH_TEST);
	command->enables.push_back(GL_BLEND);
	command->viewport = viewport;
	command->shader = terrainShader;
	command->vao = terrain->vao();
	commands.push_back(command);
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
	auto size = window->bufferSize();
	// update viewport size
	viewport->resize(0, 0, size.x, size.y);
	// update projection
	projection = glm::perspective(FIELD_OF_VIEW, float32_t(size.x) / float32_t(size.y), NEAR_PLANE, FAR_PLANE);
}

void deserialize_frame(StreamBuffer::Shared stream) {
	auto frame = Frame::alloc();
	stream >> frame;
	add_frame(frame);
	return;
}

std::vector<uint8_t> serialize_input(Input::Shared input) {
	auto stream = StreamBuffer::alloc();
	stream << input;
	return stream->buffer();
}

std::tuple<Frame::Shared, Frame::Shared, float32_t> get_frames(std::time_t now) {
	//
	//  find closest frames on either side of the delayed time such that:
	//
	//	  frame0->timestamp() < (now - delay) < frame0->timestamp()
	//
	//			  interpolation window
	//					  |
	//					  V
	//				 -----------
	//				 |		 |
	//  - - - - - - - - - - - - - - - - - - - - - - - - -
	//  |	   |	   |   x   |	   |   x   |	   |
	//
	//  0	   1	   2	   3	   4	   5	   6	<- frames
	//				 |		 |
	//				 -----------
	//					  ^			   ^
	//					  |			   |
	//				   delayed		  actual
	//					time			 time
	//					  -----------------
	//							  ^
	//							  |
	//					 interpolation delay

	// we need at least 2 frames
	if (frames.size() < 2) {
		return std::make_tuple(nullptr, nullptr, 0);
	}

	// get last frame index
	int32_t lastFrameIndex = int32_t(frames.size()) - 1;

	// delay timestamp for interpolation "window"
	auto delayed = now - Game::INTERPOLATION_DELAY;

	// find closest frames on each side of the current time

	// BEST CASE: we have no loss / delay and a window of one frame size
	// AVERAGE CASE: we have a bit of loss / delay and a larger window
	// WORST CASE: there are none "ahead" and we are working with stale data

	// find the closest frame to interpolate from
	// BASE CASE: it's the oldest frame in the buffer
	int32_t fromIndex = frames.size() - 1;
	for (uint32_t i = 0; i < frames.size(); i++) {
		auto frame = frames[i];
		if (delayed - frame->timestamp() > 0) {
			fromIndex = i;
			break;
		}
	}

	// find the closest frame to interpolate to
	// BASE CASE: it's the newest frame in the buffer
	int32_t toIndex = 0;
	for (int32_t i = lastFrameIndex; i >= 0; i--) {
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
		if (toIndex == lastFrameIndex && fromIndex == lastFrameIndex) {
			// not enough time has passed, not ready to interpolate
			return std::make_tuple(nullptr, nullptr, 0);
		}
	}

	// remove any frames that have expired
	if (int32_t(frames.size()) != fromIndex+1) {
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

	// process events
	window->processEvents();

	// poll for input
	auto input = window->poll();
	// send input to server
	for (auto i : input) {
		client->send(DeliveryType::RELIABLE, serialize_input(i));
	}
	// TODO: interpolate from input

	// get frames to interpolate between
	Frame::Shared a, b;
	float32_t t;
	std::tie(a, b, t) = get_frames(now);

	// no frames to interpolate
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

	// draw terrain
	Renderer::render(render_terrain(terrain));

	// draw players
	for (auto iter : frame->players()) {
		auto player = iter.second;
		Renderer::render(render_phong(cube, player->transform()->matrix()));
	}

	// swap back buffer
	window->swapBuffers();
}

void load_viewport() {
	auto size = window->bufferSize();
	viewport = Viewport::alloc(0, 0, size.x, size.y);
}

void load_camera() {
	camera = Transform::alloc();
	camera->setTranslation(glm::normalize(glm::vec3(0, 1, 1)) * DEFAULT_DISTANCE);
	camera->rotateLocal(-M_PI/4.0, glm::vec3(1, 0, 0));
}

glm::vec3 mouse_to_world(glm::vec2 position) {
	auto mvp = projection * camera->viewMatrix();
	auto mvpInverse = glm::inverse(mvp);
	// map window coords to range [0 .. 1]
	// TODO: fix for high DPI?
	auto size = window->size();
	auto nx = position.x / size.x;
	auto ny = (size.y - position.y) / size.y;
	auto nz = 0.0;
	// map to range of [-1 .. 1]
	auto input = glm::vec4(
		(nx * 2.0) - 1.0,
		(ny * 2.0) - 1.0,
		(nz * 2.0) - 1.0,
		1.0);
	auto output = mvpInverse * input;
	if (output.w == 0.0) {
		LOG_ERROR("w == 0.0");
		return glm::vec3();
	}
	auto world = glm::vec3(
		output.x / output.w,
		output.y / output.w,
		output.z / output.w);
	return glm::normalize(world - camera->translation());
}

Input::Shared move_to_click(
	const MouseButtonEvent& event,
	const std::map<Button, ButtonState>& mouseState,
	const std::map<Key, KeyState>& keyboardState) {
	if (event.button == Button::LEFT) {
		auto direction = mouse_to_world(event.position);
		auto origin = camera->translation();
		auto intersection = terrain->intersect(direction, origin);
		if (intersection.hit) {
			auto input = Input::alloc(InputType::MOVE_TO);
			input->emplace("position", intersection.position);
			return input;
		}
	}
	return nullptr;
}

Input::Shared move_to_hold(
	const MouseMoveEvent& event,
	const std::map<Button, ButtonState>& mouseState,
	const std::map<Key, KeyState>& keyboardState) {
	auto button = get(mouseState, Button::LEFT);
	if (button == ButtonState::DOWN) {
		auto direction = mouse_to_world(event.position);
		auto origin = camera->translation();
		auto intersection = terrain->intersect(direction, origin);
		if (intersection.hit) {
			auto input = Input::alloc(InputType::MOVE_TO);
			input->emplace("position", intersection.position);
			return input;
		}
	}
	return nullptr;
}

Input::Shared rotate_camera(
	const MouseMoveEvent& event,
	const std::map<Button, ButtonState>& mouseState,
	const std::map<Key, KeyState>& keyboardState) {
	// rotate with mouse button
	auto button = get(mouseState, Button::RIGHT);
	if (button == ButtonState::DOWN) {
		float32_t xAngle = event.delta.x * X_FACTOR * (M_PI / 180.0);
		float32_t yAngle = event.delta.y * Y_FACTOR * (M_PI / 180.0);
		camera->setTranslation(glm::vec3(0, 0, 0));
		camera->rotateGlobal(xAngle, glm::vec3(0, 1, 0));
		camera->rotateLocal(yAngle, glm::vec3(1, 0, 0));
		camera->translateLocal(glm::vec3(0, 0, distance));
	}
	return nullptr;
}

Input::Shared zoom_camera(
	const MouseScrollEvent& event,
	const std::map<Button, ButtonState>& mouseState,
	const std::map<Key, KeyState>& keyboardState) {
	// translate camera
	float32_t delta = (event.delta * -SCROLL_FACTOR);
	distance += delta;
	distance = std::min(std::max(distance, MIN_DISTANCE), MAX_DISTANCE);
	camera->setTranslation(glm::vec3(0, 0, 0));
	camera->translateLocal(glm::vec3(0, 0, distance));
	return nullptr;
}

Input::Shared move(
	const KeyboardEvent event,
	const std::map<Key, KeyState>& keyboardState,
	const std::map<Button, ButtonState>& mouseState) {

	// only care about movement keys
	if (event.key != Key::SCAN_W &&
		event.key != Key::SCAN_A &&
		event.key != Key::SCAN_S &&
		event.key != Key::SCAN_D) {
		return nullptr;
	}

	// get the state of all movement keys
	auto w = get(keyboardState, Key::SCAN_W);
	auto a = get(keyboardState, Key::SCAN_A);
	auto s = get(keyboardState, Key::SCAN_S);
	auto d = get(keyboardState, Key::SCAN_D);

	if (event.type == KeyEvent::PRESS ||
		event.type == KeyEvent::RELEASE) {

		// release
		if (w == KeyState::UP &&
			a == KeyState::UP &&
			s == KeyState::UP &&
			d == KeyState::UP) {
			// move stop
			return Input::alloc(InputType::MOVE_STOP);
		}

		// press
		glm::vec3 direction(0, 0, 0);
		if (w == KeyState::DOWN) {
			direction += glm::vec3(0, 0, -1);
		}
		if (a == KeyState::DOWN) {
			direction += glm::vec3(-1, 0, 0);
		}
		if (s == KeyState::DOWN) {
			direction += glm::vec3(0, 0, 1);
		}
		if (d == KeyState::DOWN) {
			direction += glm::vec3(1, 0, 0);
		}
		// normalize direction
		if (glm::length2(direction) < M_EPSILON) {
			// move stop
			return Input::alloc(InputType::MOVE_STOP);
		}
		direction = glm::normalize(direction);
		// update movement direction
		auto input = Input::alloc(InputType::MOVE_DIRECTION);
		input->emplace("direction", direction);
		LOG_INFO("sending input: " << glm::to_string(direction));
		return input;
	}
	return nullptr;
}

Input::Shared jump(
	const KeyboardEvent event,
	const std::map<Key, KeyState>& keyboardState,
	const std::map<Button, ButtonState>& mouseState) {

	// only care about jump key
	if (event.key != Key::SCAN_SPACE) {
		return nullptr;
	}

	// only care about fresh press
	// if (event.type == KeyEvent::PRESS) {
	// 	return Input::alloc(InputType::JUMP);
	// }
	return nullptr;
}

int main(int argc, char** argv) {

	std::srand(std::time(0));

	// TODO: put this in window class?
	std::signal(SIGINT, signal_handler);
	std::signal(SIGQUIT, signal_handler);
	std::signal(SIGTERM, signal_handler);

	window = SDL2Window::alloc("ENet Example", 1024, 768);
	mouse = window->mouse();
	mouse->add(rotate_camera);
	mouse->add(zoom_camera);
	mouse->add(move_to_click);
	mouse->add(move_to_hold);

	keyboard = window->keyboard();
	keyboard->add(move);
	keyboard->add(jump);

	load_viewport();
	load_camera();
	load_cube();
	load_shaders();
	load_axes();

	client = Client::alloc();

	if (client->connect(HOST, PORT)) {
		return 1;
	}

	terrain = Terrain::alloc(
		"resources/images/rock.png",
		"resources/images/grass.png",
		"resources/images/dgrass.png",
		"resources/images/dirt.png");
	terrain->generateGeometry(512, 512, 0.02, 2.0, 0.01);
	terrain->generateVAO();
	terrain->transform()->translateLocal(glm::vec3(0, -3, 0));
	terrain->transform()->setScale(3.0);

	while (!window->shouldClose() && !quit) {

		std::time_t now = Time::timestamp();

		// process the frame
		process_frame(now);

		// poll for messages
		auto messages = client->poll();

		// process messages
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
}
