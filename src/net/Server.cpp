#include "net/Server.h"

#include "Log.h"

Server::Shared Server::alloc() {
	return std::make_shared<Server>();
}

Server::Server()
	: host_(NULL) {
	// initialize enet
	// TODO: prevent this from being called multiple times
	if (enet_initialize() != 0) {
		LOG_ERROR("An error occurred while initializing ENet");
		return;
	}
}

Server::~Server() {
	// stop the server if we haven't already
	stop();
	// destroy the host
	// NOTE: safe to call when host is NULL
	enet_host_destroy(host_);
	host_ = NULL;
	// TODO: prevent this from being called multiple times
	enet_deinitialize();
}

bool Server::start(uint32_t port) {
	// create address
	ENetAddress address;
	enet_address_set_host(&address, "localhost");
	address.port = port;
	// create host
	host_ = enet_host_create(
		&address, // the address to bind the server host to
		MAX_CONNECTIONS, // allow up to N clients and/or outgoing connections
		NUM_CHANNELS, // allow up to N channels to be used
		0, // assume any amount of incoming bandwidth
		0); // assume any amount of outgoing bandwidth
	// check if creation was successful
	// NOTE: this only fails if malloc fails inside `enet_host_create`
	if (host_ == NULL) {
		LOG_ERROR("An error occurred while trying to create an ENet server host");
		return 1;
	}
	return 0;
}

bool Server::stop() {
	if (clients_.empty()) {
		// no clients to disconnect from
		return 0;
	}
	// attempt to gracefully disconnect all clients
	LOG_DEBUG("Disconnecting clients...");
	for (auto iter : clients_) {
		auto client = iter.second;
		// disconnect the client
		enet_peer_disconnect(client, 0);
	}
	// track how many clients to disconnect
	uint32_t remainingClients = clients_.size();
	// wait for the disconnections to be acknowledged
	ENetEvent event;
	while (enet_host_service(host_, &event, CONNECTION_TIMEOUT_MS) > 0) {
		if (event.type == ENET_EVENT_TYPE_RECEIVE) {
			// throw away any received packets during disconnect
			LOG_DEBUG("Discarding received packet");
			enet_packet_destroy(event.packet);
		} else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
			// disconnect successful
			remainingClients--;
			LOG_DEBUG("Disconnection of client_"
				<< event.peer->incomingPeerID
				<< " succeeded, "
				<< remainingClients
				<< " remaining");
			if (remainingClients <= 0) {
				break;
			}
		}
	}
	// clear clients
	clients_ = std::map<uint32_t, ENetPeer*>();
	return 0;
}

uint32_t Server::numClients() const {
	return clients_.size();
}

void Server::broadcast(const void* data, uint32_t byteSize) const {
	if (clients_.empty()) {
		// no clients to broadcast to
		return;
	}
	const uint32_t CHANNEL = 0;
	// create the packet
	ENetPacket* packet = enet_packet_create(
		data,
		byteSize + 1,
		ENET_PACKET_FLAG_RELIABLE);
	for (auto iter : clients_) {
		auto client = iter.second;
		// send the packet to the client
		enet_peer_send(client, CHANNEL, packet);
	}
	// flush / send the packet queue
	enet_host_flush(host_);
}

std::vector<Message::Shared> Server::poll() {
	std::vector<Message::Shared> msgs;
	ENetEvent event;
	while (true) {
		// poll with a zero timeout
		int32_t res = enet_host_service(host_, &event, 0);
		if (res > 0) {
			// event occured
			if (event.type == ENET_EVENT_TYPE_RECEIVE) {
				// received a packet
				LOG_DEBUG("A packet of length "
					<< event.packet->dataLength
					<< " containing `"
					<< event.packet->data
					<< "` was received from client_"
					<< event.peer->incomingPeerID);
				auto msg = Message::alloc(MessageType::DATA, (const char*)(event.packet->data));
				msgs.push_back(msg);
				// destroy packet payload
				enet_packet_destroy(event.packet);
			} else if (event.type == ENET_EVENT_TYPE_CONNECT) {
				// client connected
				auto msg = Message::alloc(MessageType::CONNECT);
				msgs.push_back(msg);
				// add to client map
				clients_[event.peer->incomingPeerID] = event.peer;

				LOG_DEBUG("Client has connected from "
					<< addressToString(&event.peer->address)
					<< ", "
					<< clients_.size()
					<< " connected clients");
			} else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
				// client disconnected
				auto msg = Message::alloc(MessageType::DISCONNECT);
				msgs.push_back(msg);
				// remove from client map
				clients_.erase(event.peer->incomingPeerID);
				LOG_DEBUG("Client has disconnected from "
					<< addressToString(&event.peer->address)
					<< ", "
					<< clients_.size()
					<< " clients remaining");
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
