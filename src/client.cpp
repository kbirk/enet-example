#include "Common.h"
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
#include "window/Window.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <algorithm>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>

typedef std::map<uint32_t, Transform::Shared> Frame;

const std::string HOST = "localhost";
const uint32_t PORT = 7000;
const time_t DISCONNECT_TIMEOUT = 5000;

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

std::vector<Frame> frames;
const uint32_t NUM_BUFFERED_FRAMES = 3;

std::vector<time_t> frameStamps;

void addFrame(const Frame& node) {
	while (frames.size() > NUM_BUFFERED_FRAMES) {
		frames.erase(frames.begin());
	}
	frames.push_back(node);
}

void addFrameStamp(time_t time) {
	while (frameStamps.size() > 20) {
		frameStamps.erase(frameStamps.begin());
	}
	frameStamps.push_back(time);
}

float32_t getFrameRate() {
	float32_t sum = 0;
	for (auto stamp : frameStamps) {
		sum += stamp;
	}
	return sum / float32_t(frameStamps.size());
}

Frame interpolateFrames(const Frame& a, const Frame& b, float32_t t) {
	auto frame = Frame();
	for (auto iter : a) {
		auto id = iter.first;
		auto ta = iter.second;
		// check that another frame exists for it.
		auto match = b.find(id);
		if (match == b.end()) {
			continue;
		}
		auto tb = match->second;
		// interpolate between frames
		auto transform = Transform::alloc();
		transform->setTranslation(glm::lerp(ta->translation(), tb->translation(), t));
		transform->setRotation(glm::slerp(ta->rotation(), tb->rotation(), t));
		transform->setScale(glm::lerp(ta->scale(), tb->scale(), t));
		// add to frame
		frame[id] = transform;
	}
	return frame;
}

std::string shader_path(const std::string& str, const std::string& type) {
	return "resources/shaders/" + str + "." + type;
}

void load_shader() {
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
	sleepMS(DISCONNECT_TIMEOUT);
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

void handle_mouse_press(WindowEvent& event) {
	down = true;
}

void handle_mouse_release(WindowEvent& event) {
	down = false;
}

void handle_mouse_move(WindowEvent& event) {
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

void handle_mouse_wheel(WindowEvent& event) {
	float32_t y = event.originalEvent->wheel.y;
	distance += (y * -SCROLL_FACTOR * MAX_DISTANCE);
	distance = std::min(std::max(distance, MIN_DISTANCE), MAX_DISTANCE);
	camera->setTranslation(glm::vec3(0, 0, 0));
	camera->translateLocal(glm::vec3(0, 0, distance));
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
	auto frame = Frame();
	while (!stream->eof()) {
		uint32_t id = 0;
		auto transform = Transform::alloc();
		stream >> id >> transform;
		frame[id] = transform;
	}
	addFrame(frame);
	return;
}

bool process_frame(std::time_t now, std::time_t delta) {

	// don't process until we've buffered enough frames
	if (frames.size() < NUM_BUFFERED_FRAMES) {
		return false;
	}

	// update viewport and projection
	update_view();

	// handle events
	if (Window::handleEvents()) {
		return true;
	}

	// handle keyboard state
	auto states = Window::pollKeyboard();
	handle_keyboard(states);

	// get t value
	float32_t frameRate = getFrameRate();
	float32_t t = std::min(1.0f, float32_t(delta) / frameRate);

	// interpolate frame
	auto frame = interpolateFrames(frames[0], frames[1], t);

	// clear buffers
	glClearColor(0.137f, 0.137f, 0.137f, 1.0f);
	LOG_OPENGL("glClearColor");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	LOG_OPENGL("glClear");

	// draw origin
	Renderer::render(render_axes());

	// draw transforms
	for (auto iter : frame) {
		auto transform = iter.second;
		Renderer::render(render_phong(cube, transform->matrix()));
	}

	// swap back buffer
	Window::swapBuffers();

	return false;
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

	Window::on(WindowEventType::MOUSE_LEFT_RELEASE, handle_mouse_release);
	Window::on(WindowEventType::MOUSE_LEFT_PRESS, handle_mouse_press);
	Window::on(WindowEventType::MOUSE_MOVE, handle_mouse_move);
	Window::on(WindowEventType::MOUSE_WHEEL, handle_mouse_wheel);

	load_viewport();
	load_cube();
	load_shader();
	load_axes();

	client = Client::alloc();

	if (client->connect(HOST, PORT)) {
		return 1;
	}

	std::time_t last = timestamp();

	while (true) {

		std::time_t now = timestamp();
		std::time_t elapsed = now - last;

		// process the frame
		if (process_frame(now, elapsed)) {
			// exit
			break;
		}

		// poll for events
		auto messages = client->poll();

		// process events
		for (auto msg : messages) {
			if (msg->type() == MessageType::DISCONNECT) {
				// handle the disconnect
				handle_disconnect();
				now = timestamp();
				last = now;
				// ignore other messages
				break;
			} else if (msg->type() == MessageType::DATA) {
				// handle message
				auto stream = msg->stream();
				deserialize_frame(stream);
				addFrameStamp(elapsed);
				last = now;
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
