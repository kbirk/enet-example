#include "Common.h"
#include "protocol/Command.h"
#include "log/Log.h"
#include "net/Message.h"
#include "net/Server.h"
#include "render/Transform.h"
#include "serial/StreamBuffer.h"

#include "glm/glm.hpp"

#include <algorithm>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>

const uint32_t PORT = 7000;
const time_t STEP_MS = 100;
const time_t STEPS_PER_SEC = 1000 / STEP_MS;

bool quit = false;

Server::Shared server;
std::map<uint32_t, Transform::Shared> frame;

void signal_handler(int32_t signal) {
	LOG_DEBUG("Caught signal: " << signal << ", shutting down...");
	quit = true;
}

Transform::Shared create_transform() {
	auto transform = Transform::alloc();
	// transform->setScale(0.5);
	return transform;
}

std::vector<uint8_t> serialize_frame() {
	if (frame.empty()) {
		return std::vector<uint8_t>();
	}
	StreamBuffer stream;
	for (auto iter : frame) {
		auto id = iter.first;
		auto transform = iter.second;
		stream << id << transform;
	}
	return stream.buffer();
}

Command deserialize_command(const uint8_t* src, uint32_t numBytes) {
	StreamBuffer stream(src, numBytes);
	auto command = Command();
	stream >> command;
	return command;
}

void apply_command(Transform::Shared& transform, const Command& command) {
	switch (command.type) {
		case CommandType::MOVE:
			transform->translateGlobal(command.direction);
			break;
	}
}

void process_frame(std::time_t stamp, std::time_t delta) {
	float32_t pi2 = 2.0 * M_PI;
	float32_t angle = std::fmod(((float64_t(stamp) / 2000.0) * pi2), pi2);
	// LOG_DEBUG("Setting angle to: " << angle << " radians for time of: " << stamp);
	auto axis = glm::vec3(1, 1, 1);
	for (auto iter : frame) {
		auto id = iter.first;
		auto transform = iter.second;
		if (id > server->numClients()) {
			// rotate and translate non-clients
			transform->setRotation(angle, axis);
			transform->setTranslation(glm::vec3(std::sin(angle) * 3.0, 0.0, 0.0));
		}
	}
}

int main(int argc, char** argv) {

	std::srand(std::time(0));

	std::signal(SIGINT, signal_handler);
	std::signal(SIGQUIT, signal_handler);
	std::signal(SIGTERM, signal_handler);

	server = Server::alloc();

	uint32_t ARBITRARY_ID = 256; // high enough not to conflict with a client id
	frame[ARBITRARY_ID] = create_transform();

	if (server->start(PORT)) {
		return 1;
	}

	std::time_t last = timestamp();

	auto frameCount = 0;

	while (true) {

		std::time_t stamp = timestamp();

		// poll for events
		auto messages = server->poll();

		// process events
		for (auto msg : messages) {
			if (msg->type() == MessageType::CONNECT) {

				LOG_DEBUG("Connection from client_" << msg->id() << " received");
				frame[msg->id()] = Transform::alloc();

			} else if (msg->type() == MessageType::DISCONNECT) {

				LOG_DEBUG("Connection from client_" << msg->id() << " lost");
				frame.erase(msg->id());

			} else if (msg->type() == MessageType::DATA) {

				LOG_DEBUG("Message recieved from client");
				auto command = deserialize_command(
					msg->packet()->data(),
					msg->packet()->numBytes());
				apply_command(frame[msg->id()], command);

			}
		}

		// process the frame
		process_frame(stamp, stamp - last);

		// broadcast to all clients
		auto msg = serialize_frame();
		server->broadcast(PacketType::RELIABLE, Packet::alloc(&msg[0], msg.size()));

		// check if exit
		if (quit) {
			break;
		}

		// determine elapsed time to calc sleep for next frame
		std::time_t now = timestamp();
		std::time_t elapsed = now - stamp;

		// sleep
		if (elapsed < STEP_MS) {
			sleepMS(STEP_MS - elapsed);
		}

		// debug
		if (frameCount % STEPS_PER_SEC == 0) {
			LOG_INFO("Tick of " << (now - last) << "ms processed in " << elapsed << "ms");
		}
		last = now;
		frameCount++;
	}

	// stop server and disconnect all clients
	server->stop();
}
