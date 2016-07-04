#include "Common.h"
#include "Log.h"
#include "net/Client.h"
#include "net/Message.h"

#include <algorithm>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>

const std::string HOST = "localhost";
const uint32_t PORT = 7000;
const uint64_t STEP_MS = 1000;

bool quit = false;

Client::Shared client;

void signal_handler(int32_t signal) {
	LOG_DEBUG("Caught signal: " << signal << ", shutting down...");
	quit = true;
}

int main(int argc, char** argv) {

	std::srand(std::time(0));

	std::signal(SIGINT, signal_handler);
	std::signal(SIGQUIT, signal_handler);
	std::signal(SIGTERM, signal_handler);

	client = Client::alloc();

	if (client->connect(HOST, PORT)) {
		return 1;
	}

	std::time_t last = timestamp();

	while (!quit) {

		std::time_t stamp = timestamp();

		// poll for events
		auto messages = client->poll();

		// process events
		for (auto msg : messages) {
			if (msg->type() == MessageType::DISCONNECT) {
				// attempt to reconnect
				while (!quit) {
					LOG_DEBUG("Attempting to re-connect to server...");
					if (!client->connect(HOST, PORT)) {
						// success
						stamp = timestamp();
						last = stamp;
						break;
					}
				}
				// ignore other messages
				break;
			}
		}

		// send message to server
		std::string msg = "I am a client, this is my msg id=" + std::to_string(std::rand());
		client->send(msg.c_str(), msg.size());

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

	client->disconnect();
}
