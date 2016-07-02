#include <enet/enet.h>
#include <iostream>
#include <cstring>
#include <string>
#include <ctime>

const char* HOST = "localhost";
const uint32_t PORT = 7000;
const uint32_t STEP_MS = 16;

int main(int argc, char** argv) {

	std::srand(std::time(0));

	if (enet_initialize() != 0) {
		std::cout << "An error occurred while initializing ENet" << std::endl;
		return 1;
	}

	ENetAddress address;
	enet_address_set_host(&address, HOST);
	address.port = PORT;

	ENetHost* server = enet_host_create(
		&address, // the address to bind the server host to
		32, // allow up to 32 clients and/or outgoing connections
		2, // allow up to 2 channels to be used, 0 and 1
		0, // assume any amount of incoming bandwidth
		0); // assume any amount of outgoing bandwidth

	if (server == NULL) {
		std::cout << "An error occurred while trying to create an ENet server host" << std::endl;
		return 1;
	}

	ENetEvent event;
	ENetPacket* packet;
	std::string msg;

	// Step
	while (enet_host_service(server, &event, STEP_MS) >= 0) {
		switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				std::cout << "A new client connected from "
					<< event.peer->address.host << ":"
					<< event.peer->address.port
					<< std::endl;
				break;

			case ENET_EVENT_TYPE_RECEIVE:
				std::cout << "A packet of length " << event.packet->dataLength
					<< " containing `" << event.packet->data
					<< "` was received from " << event.peer->incomingPeerID
					<< " on channel " << event.channelID
					<< std::endl;
				// Create message
				msg = "I am the server, this is my response id=" + std::to_string(std::rand());
				// Create a reliable packet
				packet = enet_packet_create(
					msg.c_str(),
					msg.size() + 1,
					ENET_PACKET_FLAG_RELIABLE);
				// Send the packet to the peer over channel id 0
				enet_peer_send(event.peer, 0, packet);
				// One could just use enet_host_service() instead
				enet_host_flush(server);
				// Clean up the packet now that we're done using it.
				enet_packet_destroy(event.packet);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				std::cout << event.peer->data << " disconnected" << std::endl;
				break;

			case ENET_EVENT_TYPE_NONE:
				break;

		}
	}

	enet_host_destroy(server);

	enet_deinitialize();
}
