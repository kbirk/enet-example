#include "Common.h"
#include "Log.h"
#include "net/Message.h"
#include "net/Server.h"

#include <algorithm>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>

const uint32_t PORT = 7000;
const uint64_t STEP_MS = 2000;

bool quit = false;

Server::Shared server;

void signal_handler(int32_t signal) {
	LOG_DEBUG("Caught signal: " << signal << ", shutting down...");
	quit = true;
}

int main(int argc, char** argv) {

	std::srand(std::time(0));

	std::signal(SIGINT, signal_handler);
	std::signal(SIGQUIT, signal_handler);
	std::signal(SIGTERM, signal_handler);

	server = Server::alloc();

	if (server->start(PORT)) {
		return 1;
	}

	std::time_t last = timestamp();

	while (!quit) {

		std::time_t stamp = timestamp();

		// poll for events
		auto messages = server->poll();

		// process events
		if (!messages.empty()) {
			LOG_INFO(messages.size() << " messages received");
		}

		// broadcast to all clients
		std::string msg = "I am the server this is my response=" + std::to_string(std::rand());
		server->broadcast(msg.c_str(), msg.size());

		// determine elapsed time to calc sleep for next frame
		std::time_t now = timestamp();
		std::time_t elapsed = now - stamp;

		// sleep
		if (elapsed < STEP_MS) {
			sleepMS(STEP_MS - elapsed);
		}

		// debug
		now = timestamp();
		LOG_INFO("Tick of " << (now - last) << "ms... ");
		last = now;
	}

	server->stop();
}
