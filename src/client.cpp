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
#include "net/Client.h"
#include "net/Message.h"
#include "render/Node.h"
#include "render/RenderCommand.h"
#include "render/Renderer.h"
#include "render/Transform.h"
#include "serial/Serialization.h"
#include "window/Window.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <algorithm>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>

const std::string HOST = "localhost";
const uint32_t PORT = 7000;
const time_t STEP_MS = 100;
const time_t FRAME_MS = 0;

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

VertexFragmentShader::Shared shader;
VertexArrayObject::Shared cube;
VertexArrayObject::Shared x;
VertexArrayObject::Shared y;
VertexArrayObject::Shared z;
Viewport::Shared viewport;
glm::mat4 projection;
Client::Shared client;
Node::Shared scene;
Node::Shared camera;

std::string shader_path(const std::string& str, const std::string& type) {
	return "resources/shaders/" + str + "." + type;
}

void load_shader() {
	shader = VertexFragmentShader::alloc();
	shader->create(
		shader_path("flat", "vert"),
		shader_path("flat", "frag"));
}

void load_cube() {
	// positions
	auto positions = VertexBufferObject::alloc();
	positions->upload(Cube::positions());
	// uvs
	auto uvs = VertexBufferObject::alloc();
	uvs->upload(Cube::uvs());
	// indices
	auto indices = ElementArrayBufferObject::alloc();
	indices->upload(Cube::indices());
	// vao
	cube = VertexArrayObject::alloc();
	cube->attach(positions, VertexAttributePointer::alloc(0, 3, GL_FLOAT));
	cube->attach(uvs, VertexAttributePointer::alloc(1, 2, GL_FLOAT));
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
	command->shader = shader;
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
	command->shader = shader;
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
	sleepMS(STEP_MS);
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

void deserialize(const uint8_t* src, uint32_t totalBytes) {
	if (totalBytes == 0) {
		return;
	}
	scene->clearChildren();
	uint32_t size = 0;
	while (size < totalBytes) {
		auto node = Node::alloc("child");
		size = node->deserialize(src, size);
		scene->addChild(node);
	}
	return;
}

bool process_frame(std::time_t now, std::time_t delta) {

	// update viewport and projection
	update_view();

	// handle events
	if (Window::handleEvents()) {
		return true;
	}

	// clear buffers
	glClearColor(0.137f, 0.137f, 0.137f, 1.0f);
	LOG_OPENGL("glClearColor");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	LOG_OPENGL("glClear");

	// draw origin
	Renderer::render(render_axes());

	// draw
	for (auto child : scene->children()) {
		Renderer::render(render_flat(cube, child->matrix()));
	}

	// swap back buffer
	Window::swapBuffers();

	return false;
}

void load_viewport() {
	auto size = Window::size();
	viewport = Viewport::alloc(0, 0, size.x, size.y);
	camera = Node::alloc("camera");
	camera->setTranslation(glm::vec3(0, 0.0, DEFAULT_DISTANCE));
}

void handle_mouse_down(WindowEvent event) {
	down = true;
}

void handle_mouse_up(WindowEvent event) {
	down = false;
}

void handle_mouse_move(WindowEvent event) {
	int32_t dx = event.originalEvent.motion.xrel;
	int32_t dy = event.originalEvent.motion.yrel;
	if (down) {
		float32_t xAngle = dx * X_FACTOR * (M_PI / 180.0);
		float32_t yAngle = dy * Y_FACTOR * (M_PI / 180.0);
		camera->setTranslation(glm::vec3(0, 0, 0));
		camera->rotateGlobal(xAngle, glm::vec3(0, 1, 0));
		camera->rotateLocal(yAngle, glm::vec3(1, 0, 0));
		camera->translateLocal(glm::vec3(0, 0, distance));
	}
}

void mouse_wheel(WindowEvent event) {
	float32_t y = event.originalEvent.wheel.y;
	distance += (y * -SCROLL_FACTOR * MAX_DISTANCE);
	distance = std::min(std::max(distance, MIN_DISTANCE), MAX_DISTANCE);
	camera->setTranslation(glm::vec3(0, 0, 0));
	camera->translateLocal(glm::vec3(0, 0, distance));
}

int main(int argc, char** argv) {

	std::srand(std::time(0));

	std::signal(SIGINT, signal_handler);
	std::signal(SIGQUIT, signal_handler);
	std::signal(SIGTERM, signal_handler);

	Window::setup();

	Window::on(WindowEventType::MOUSE_LEFT_UP, handle_mouse_up);
	Window::on(WindowEventType::MOUSE_LEFT_DOWN, handle_mouse_down);
	Window::on(WindowEventType::MOUSE_MOVE, handle_mouse_move);
	Window::on(WindowEventType::MOUSE_WHEEL, mouse_wheel);

	client = Client::alloc();

	load_viewport();
	load_cube();
	load_shader();
	load_axes();

	scene = Node::alloc("scene");

	if (client->connect(HOST, PORT)) {
		return 1;
	}

	std::time_t lastFrame = timestamp();
	std::time_t lastStep = lastFrame;

	auto frame = 0;

	while (true) {

		std::time_t now = timestamp();
		std::time_t frameElapsed = now - lastFrame;
		std::time_t stepElapsed = now - lastStep;

		// poll at step
		if (stepElapsed >= STEP_MS) {
			// poll for events
			auto messages = client->poll();

			// process events
			for (auto msg : messages) {
				if (msg->type() == MessageType::DISCONNECT) {
					// handle the disconnect
					handle_disconnect();
					now = timestamp();
					lastFrame = now;
					// ignore other messages
					break;
				} else if (msg->type() == MessageType::DATA) {
					// handle message
					const uint8_t* data = (const uint8_t*)(msg->packet()->data());
					uint32_t numBytes = msg->packet()->numBytes();
					deserialize(data, numBytes);
				}
			}
			lastStep = now;
		}

		if (frameElapsed >= FRAME_MS) {
			// process the frame
			if (process_frame(now, timestamp() - now)) {
				// exit
				break;
			}
			lastFrame = now;
		}

		// send message to server
		// std::string msg = "msg id=" + std::to_string(std::rand());
		// client->send(PacketType::RELIABLE, Packet::alloc(msg.c_str(), msg.size()));

		// check if exit
		if (quit) {
			break;
		}

		// increment frame
		frame++;

		// determine elapsed time to calc sleep for next frame
		now = timestamp();
		frameElapsed = now - lastFrame;
		stepElapsed = now - lastStep;

		std::time_t frameRemaining = FRAME_MS - frameElapsed;
		std::time_t stepRemaining = STEP_MS - stepElapsed;

		if (frameRemaining < stepRemaining) {
			// closer to next frame
			if (frameRemaining > 0) {
				sleepMS(frameRemaining);
			}
		} else {
			// closer to next step
			if (stepRemaining > 0) {
				sleepMS(stepRemaining);
			}
		}

	}

	client->disconnect();

	Window::teardown();
}
