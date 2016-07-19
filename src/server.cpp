#include "Common.h"
#include "log/Log.h"
#include "net/Message.h"
#include "net/Server.h"
#include "render/Transform.h"

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
std::vector<Transform::Shared> transforms;

void signal_handler(int32_t signal) {
	LOG_DEBUG("Caught signal: " << signal << ", shutting down...");
	quit = true;
}

Transform::Shared create_transform() {
	auto transform = Transform::alloc();
	// transform->setScale(0.5);
	return transform;
}

std::vector<uint8_t> serialize(const std::vector<Transform::Shared>& tranforms) {
	if (transforms.empty()) {
		return std::vector<uint8_t>();
	}
	uint32_t byteSize = transforms[0]->serialize(nullptr);
	uint32_t size = 0;
	std::vector<uint8_t> data(byteSize * transforms.size());
	for (auto transform : transforms) {
		size = transform->serialize(&data[0], size);
	}
	return data;
}

void process_frame(std::time_t stamp, std::time_t delta) {
	float32_t pi2 = 2.0 * M_PI;
	float32_t angle = std::fmod(((float64_t(stamp) / 2000.0) * pi2), pi2);
	// LOG_DEBUG("Setting angle to: " << angle << " radians for time of: " << stamp);
	auto axis = glm::vec3(1, 1, 1);
	for (auto transform : transforms) {
		transform->setRotation(angle, axis);
		transform->setTranslation(glm::vec3(std::sin(angle) * 3.0, 0.0, 0.0));
	}
}

int main(int argc, char** argv) {

	std::srand(std::time(0));

	std::signal(SIGINT, signal_handler);
	std::signal(SIGQUIT, signal_handler);
	std::signal(SIGTERM, signal_handler);

	server = Server::alloc();

	transforms.push_back(create_transform());

	if (server->start(PORT)) {
		return 1;
	}

	std::time_t last = timestamp();

	auto frame = 0;

	while (true) {

		std::time_t stamp = timestamp();

		// poll for events
		auto messages = server->poll();

		// process events
		if (!messages.empty()) {
			LOG_INFO(messages.size() << " messages received");
		}

		// process the frame
		process_frame(stamp, stamp - last);

		// broadcast to all clients
		auto msg = serialize(transforms);
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
		if (frame % STEPS_PER_SEC == 0) {
			LOG_INFO("Tick of " << (now - last) << "ms processed in " << elapsed << "ms");
		}
		last = now;
		frame++;
	}

	server->stop();
}
