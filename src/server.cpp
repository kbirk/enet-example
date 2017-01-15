#include "Common.h"
#include "game/Common.h"
#include "game/Frame.h"
#include "game/Player.h"
#include "log/Log.h"
#include "math/Transform.h"
#include "net/DeliveryType.h"
#include "net/Message.h"
#include "net/Server.h"
#include "serial/StreamBuffer.h"
#include "time/Time.h"

#include "glm/glm.hpp"

#include <algorithm>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>

const uint32_t PORT = 7000;

bool quit = false;

Server::Shared server;
Frame::Shared frame;

void signal_handler(int32_t signal) {
	LOG_DEBUG("Caught signal: " << signal << ", shutting down...");
	quit = true;
}

void process_input(Player::Shared player, Input::Shared input) {
	player->state()->handleInput(input);
}

std::vector<uint8_t> serialize_frame(const Frame::Shared& frame) {
	auto stream = StreamBuffer::alloc();
	stream << frame;
	return stream->buffer();
}

Input::Shared deserialize_input(StreamBuffer::Shared stream) {
	auto input = Input::alloc(0);
	stream >> input;
	return input;
}

void process_frame(const Frame::Shared& frame, std::time_t now, std::time_t last) {
	auto pi2 = 2.0 * M_PI;
	auto factor = now / float64_t(Time::seconds(2));
	auto angle = std::fmod(factor * pi2, pi2);
	// LOG_DEBUG("Setting angle to: " << angle << " radians for time of: " << now);
	auto axis = glm::vec3(1, 1, 1);
	for (auto iter : frame->players()) {
		auto id = iter.first;
		auto player = iter.second;
		if (id > server->numClients()) {
			// rotate and translate non-clients
			player->transform()->setRotation(angle, axis);
			player->transform()->setTranslation(glm::vec3(std::sin(angle) * 3.0, 0.0, 0.0));
		}
		// update player state
		player->update(now - last);
	}
}

// void send_client_info(uint32_t id) {
// 	StreamBuffer stream;
// 	stream << id;
// 	auto bytes = stream.buffer();
// 	server->send(id, DeliveryType::RELIABLE, bytes);
// }

int main(int argc, char** argv) {

	std::srand(std::time(0));

	std::signal(SIGINT, signal_handler);
	std::signal(SIGQUIT, signal_handler);
	std::signal(SIGTERM, signal_handler);

	frame = Frame::alloc();
	// TEMP: high enough ID not to conflict with a client id
	frame->addPlayer(256, Player::alloc());

	server = Server::alloc();
	if (server->start(PORT)) {
		return 1;
	}

	std::time_t last = Time::timestamp();

	auto frameCount = 0;

	while (true) {

		std::time_t now = Time::timestamp();

		// poll for events
		auto messages = server->poll();

		// process events
		for (auto msg : messages) {

			switch (msg->type()) {

				case MessageType::CONNECT:
					LOG_DEBUG("Connection from client_" << msg->id() << " received");
					frame->addPlayer(msg->id(), Player::alloc());
					break;

				case MessageType::DISCONNECT:

					LOG_DEBUG("Connection from client_" << msg->id() << " lost");
					frame->removePlayer(msg->id());
					break;

				case MessageType::DATA:

					LOG_DEBUG("Message received from client `" << msg->id() << "`");
					auto input = deserialize_input(msg->stream());
					auto players = frame->players();
					auto player = players[msg->id()];
					process_input(player, input);
					break;
			}
		}

		// process the frame
		process_frame(frame, now, last);

		// update frame timestmap
		frame->setTimestamp(now);

		// broadcast frame to all clients
		server->broadcast(DeliveryType::RELIABLE, serialize_frame(frame));

		// check if exit
		if (quit) {
			break;
		}

		// determine elapsed frame time to calc sleep for next frame
		std::time_t elapsed = Time::timestamp() - now;

		// sleep until next frame step
		if (elapsed < Game::STEP_DURATION) {
			Time::sleep(Game::STEP_DURATION - elapsed);
		}

		// debug
		if (frameCount % Game::STEPS_PER_SEC == 0) {
			LOG_INFO("Tick of " << Time::format(now - last) << " processed in " << Time::format(elapsed));
		}

		last = now;
		frameCount++;
	}

	// stop server and disconnect all clients
	server->stop();
}
