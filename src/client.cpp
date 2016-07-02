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

	ENetHost* client = enet_host_create(
		NULL, // create a client host
		1, // only allow 1 outgoing connection
		2, // allow up 2 channels to be used, 0 and 1
		0, // assume any amount of incoming bandwidth
		0); // assume any amount of outgoing bandwidth

	if (client == NULL) {
		std::cout << "An error occurred while trying to create an ENet client host" << std::endl;
		return 1;
	}

	// Connect to the host
	ENetAddress address;
	enet_address_set_host(&address, HOST);
	address.port = PORT;

	// Initiate the connection, allocating the two channels 0 and 1.
	ENetPeer* peer = enet_host_connect(client, &address, 2, 0);
	if (peer == NULL) {
		std::cout << "No available peers for initiating an ENet connection" << std::endl;
		return 1;
	}

	ENetEvent event;

	// Wait up to 5 seconds for the connection attempt to succeed.
	if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
		std::cout << "Connection to `" << HOST << ":" << PORT << "` succeeded" << std::endl;

		std::string msg = "A hoy hoy, I am a client, this is my initial message id=" + std::to_string(std::rand());

		// Create a reliable packet
		ENetPacket* packet = enet_packet_create(
			msg.c_str(),
			msg.size(),
			ENET_PACKET_FLAG_RELIABLE);

		// Send the packet to the peer over channel id 0
		enet_peer_send(peer, 0, packet);

		// One could just use enet_host_service() instead
		enet_host_flush(client);

		// Step
		while (enet_host_service(client, &event, STEP_MS) >= 0) {
			switch (event.type) {

				case ENET_EVENT_TYPE_RECEIVE:
					std::cout << "A packet of length " << event.packet->dataLength
						<< " containing `" << event.packet->data
						<< "` was received from " << event.peer->incomingPeerID
						<< " on channel " << event.channelID
						<< std::endl;

					// Create message
					msg = "I am a client, this is my response id=" + std::to_string(std::rand());
					// Create a reliable packet
					packet = enet_packet_create(
						msg.c_str(),
						msg.size() + 1,
						ENET_PACKET_FLAG_RELIABLE);
					// Send the packet to the peer over channel id 0
					enet_peer_send(event.peer, 0, packet);
					// One could just use enet_host_service() instead
					enet_host_flush(client);
					// Clean up the packet now that we're done using it.
					enet_packet_destroy(event.packet);
					break;

				case ENET_EVENT_TYPE_CONNECT:
				case ENET_EVENT_TYPE_DISCONNECT:
				case ENET_EVENT_TYPE_NONE:
					break;

			}
		}

		// Disconnect
		enet_peer_disconnect(peer, 0);

		// Allow up to 3 seconds for the disconnect to succeed
		// and drop any packets received packets.
		while (enet_host_service(client, &event, 3000) > 0) {
			switch (event.type) {
				case ENET_EVENT_TYPE_RECEIVE:
					enet_packet_destroy(event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					std::cout << "Disconnection succeeded" << std::endl;
					break;

				case ENET_EVENT_TYPE_NONE:
				case ENET_EVENT_TYPE_CONNECT:
					break;
			}
		}
		// We've arrived here, so the disconnect attempt didn't
		// succeed yet.  Force the connection down
		enet_peer_reset(peer);

	} else {
		// Either the 5 seconds are up or a disconnect event was
		// received. Reset the peer in the event the 5 seconds
		// had run out without any significant event
		enet_peer_reset(peer);
		std::cout << "Connection to `" << HOST << ":" << PORT << "` failed" << std::endl;
	}

	enet_host_destroy(client);

	enet_deinitialize();
}
