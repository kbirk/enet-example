#include "net/Client.h"

#include "Common.h"
#include "log/Log.h"
#include "net/Packet.h"

Client::Shared Client::alloc() {
	return std::make_shared<Client>();
}

Client::Client()
	: host_(NULL)
	, server_(NULL) {
	// initialize enet
	// TODO: prevent this from being called multiple times
	if (enet_initialize() != 0) {
		LOG_ERROR("An error occurred while initializing ENet");
		return;
	}
	// create a host
	host_ = enet_host_create(
		NULL, // create a client host
		1, // only allow 1 outgoing connection
		NUM_CHANNELS, // allow up TO N channels to be used
		0, // assume any amount of incoming bandwidth
		0); // assume any amount of outgoing bandwidth
	// check if creation was successful
	// NOTE: this only fails if malloc fails inside `enet_host_create`
	if (host_ == NULL) {
		LOG_ERROR("An error occurred while trying to create an ENet client host");
	}
}

Client::~Client() {
	// disconnect if haven't already
	disconnect();
	// destroy the host
	// NOTE: safe to call when host is NULL
	enet_host_destroy(host_);
	host_ = NULL;
	// TODO: prevent this from being called multiple times
	enet_deinitialize();
}

bool Client::connect(const std::string& host, uint32_t port) {
	if (server_ != NULL) {
		LOG_DEBUG("Client is already connected to a server");
		return 0;
	}
	// set address to connect to
	ENetAddress address;
	enet_address_set_host(&address, host.c_str());
	address.port = port;
	// initiate the connection, allocating the two channels 0 and 1.
	server_ = enet_host_connect(host_, &address, NUM_CHANNELS, 0);
	if (server_ == NULL) {
		LOG_ERROR("No available peers for initiating an ENet connection");
		return 1;
	}
	// attempt to connect to the peer (server)
	ENetEvent event;
	// wait N for connection to succeed
	// NOTE: we don't need to check / destroy packets because the server will be
	// unable to send the packets without first establishing a connection
	if (enet_host_service(host_, &event, CONNECTION_TIMEOUT_MS) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT) {
		// connection successful
		LOG_DEBUG("Connection to `" << host << ":" << port << "` succeeded");
		return 0;
	}
	// failure to connect
	enet_peer_reset(server_);
	server_ = NULL;
	LOG_ERROR("Connection to `" << host << ":" << port << "` failed");
	return 1;
}

bool Client::disconnect() {
	if (server_ == NULL) {
		return 1;
	}
	LOG_DEBUG("Disconnecting from server...");
	// attempt to gracefully disconnect
	enet_peer_disconnect(server_, 0);
	// wait for the disconnect to be acknowledged
	ENetEvent event;
	bool success = false;
	while (enet_host_service(host_, &event, CONNECTION_TIMEOUT_MS) > 0) {
		if (event.type == ENET_EVENT_TYPE_RECEIVE) {
			// throw away any received packets during disconnect
			enet_packet_destroy(event.packet);
		} else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
			// disconnect successful
			LOG_DEBUG("Disconnection from server successful");
			success = true;
			break;
		}
	}
	if (!success) {
		// disconnect attempt didn't succeed yet, force close the connection
		LOG_ERROR("Disconnection was not acknowledged by server, shutdown forced");
		enet_peer_reset(server_);
	}
	server_ = NULL;
	return success;
}

bool Client::isConnected() const {
	return server_ != NULL;
}

void Client::send(PacketType type, const Packet::Shared& packet) const {
	if (server_ == NULL) {
		LOG_DEBUG("Client is not connected to any server");
		return;
	}
	uint32_t channel = 0;
	uint32_t flags = 0;
	if (type == PacketType::RELIABLE) {
		channel = RELIABLE_CHANNEL;
		flags = ENET_PACKET_FLAG_RELIABLE;
	} else {
		channel = UNRELIABLE_CHANNEL;
		flags = ENET_PACKET_FLAG_UNSEQUENCED;
	}
	// create the packet
	ENetPacket* p = enet_packet_create(
		packet->data(),
		packet->numBytes(), // + 1,
		flags);
	// send the packet to the peer
	enet_peer_send(server_, channel, p);
	// flush / send the packet queue
	enet_host_flush(host_);
}

std::vector<Message::Shared> Client::poll() {
	std::vector<Message::Shared> msgs;
	if (server_ == NULL) {
		return msgs;
	}
	ENetEvent event;
	while (true) {
		// poll with a zero timeout
		int32_t res = enet_host_service(host_, &event, 0);
		if (res > 0) {
			// event occured
			if (event.type == ENET_EVENT_TYPE_RECEIVE) {
				// received a packet
				// LOG_DEBUG("A packet of length "
				// 	<< event.packet->dataLength
				// 	<< " containing `"
				// 	<< event.packet->data
				// 	<< "` was received on channel "
				// 	<< event.channelID);
				auto msg = Message::alloc(
					MessageType::DATA,
					Packet::alloc(
						event.packet->data,
						event.packet->dataLength));
				msgs.push_back(msg);
				// destroy packet payload
				enet_packet_destroy(event.packet);

			} else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
				// server disconnected
				LOG_DEBUG("Connection to server has been lost");
				auto msg = Message::alloc(MessageType::DISCONNECT);
				msgs.push_back(msg);
				// clear peer
				server_ = NULL;
			}
		} else if (res < 0) {
			// error occured
			LOG_ERROR("Encountered error while polling");
			break;
		} else {
			// no event
			break;
		}
	}
	return msgs;
}
