#include "net/Server.h"

#include "Common.h"
#include "log/Log.h"

const uint32_t MAX_CONNECTIONS = 64;
const time_t CONNECTION_TIMEOUT_MS = 5000;

std::string addressToString(const ENetAddress* address) {
	uint8_t a = (uint8_t)(address->host);
	uint8_t b = (uint8_t)(address->host >> 8);
	uint8_t c = (uint8_t)(address->host >> 16);
	uint8_t d = (uint8_t)(address->host >> 24);
	return std::to_string(a) + "." +
		std::to_string(b) + "." +
		std::to_string(c) + "." +
		std::to_string(d) + ":" +
		std::to_string(address->port);
}

Server::Shared Server::alloc() {
	return std::make_shared<Server>();
}

Server::Server()
	: host_(nullptr) {
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
	if (host_ == nullptr) {
		LOG_ERROR("An error occurred while trying to create an ENet server host");
		return 1;
	}
	return 0;
}

bool Server::stop() {
	if (!isRunning()) {
		// no clients to disconnect from
		return 0;
	}
	// attempt to gracefully disconnect all clients
	LOG_DEBUG("Disconnecting " << numClients() << " clients...");
	for (auto iter : clients_) {
		auto client = iter.second;
		LOG_DEBUG("Disconnecting from client_" << client->incomingPeerID);
		enet_peer_disconnect(client, 0);
	}
	// wait for the disconnections to be acknowledged
	auto stamp = timestamp();
	bool success = false;
	ENetEvent event;
	while (true) {
		int32_t res = enet_host_service(host_, &event, 0);
		if (res > 0) {
			// event occured
			if (event.type == ENET_EVENT_TYPE_RECEIVE) {
				// throw away any received packets during disconnect
				LOG_DEBUG("Discarding received packet");
				enet_packet_destroy(event.packet);
			} else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
				// disconnect successful
				LOG_DEBUG("Disconnection of client_"
					<< event.peer->incomingPeerID
					<< " succeeded, "
					<< (clients_.size() - 1)
					<< " remaining");
				// remove from remaining
				clients_.erase(event.peer->incomingPeerID);
			} else if (event.type == ENET_EVENT_TYPE_CONNECT) {
				// client connected
				LOG_DEBUG("Connection accepted from client_"
					<< event.peer->incomingPeerID
					<< " during server shutdown, disconnect");
				// add and remove client
				enet_peer_disconnect(event.peer, 0);
				// add to remaining
				clients_[event.peer->incomingPeerID] = event.peer;
			}
		} else if (res < 0) {
			// error occured
			LOG_ERROR("Encountered error while polling");
			break;
		} else {
			// no event, check if finished
			if (clients_.empty()) {
				// all clients successfully disconnected
				LOG_DEBUG("Disconnection from all clients successful");
				success = true;
				break;
			}
		}
		// check timeout
		if (timestamp() - stamp > CONNECTION_TIMEOUT_MS) {
			LOG_ERROR("Disconnection was not acknowledged by server, shutdown forced");
			break;
		}
	}
	// force disconnect the remaining clients
	for (auto iter : clients_) {
		auto id = iter.first;
		auto client = iter.second;
		LOG_DEBUG("Forcibly disconnecting client_" << id);
		enet_peer_reset(client);
	}
	// clear clients
	clients_ = std::map<uint32_t, ENetPeer*>();
	// destroy the host
	enet_host_destroy(host_);
	host_ = nullptr;
	return !success;
}

bool Server::isRunning() const {
	return host_ != nullptr;
}

uint32_t Server::numClients() const {
	return host_->connectedPeers;
}

void Server::send(uint32_t id, DeliveryType type, const std::vector<uint8_t>& data) const {
	auto iter = clients_.find(id);
	if (iter == clients_.end()) {
		// no client to send to
		LOG_WARN("No connected client with id: " << id);
		return;
	}
	auto client = iter->second;
	uint32_t channel = 0;
	uint32_t flags = 0;
	if (type == DeliveryType::RELIABLE) {
		channel = RELIABLE_CHANNEL;
		flags = ENET_PACKET_FLAG_RELIABLE;
	} else {
		channel = UNRELIABLE_CHANNEL;
		flags = ENET_PACKET_FLAG_UNSEQUENCED;
	}
	// create the packet
	ENetPacket* p = enet_packet_create(
		&data[0],
		data.size(),
		flags);
	// send the packet to the peer
	enet_peer_send(client, channel, p);
	// flush / send the packet queue
	enet_host_flush(host_);
}

void Server::broadcast(DeliveryType type, const std::vector<uint8_t>& data) const {
	if (numClients() == 0) {
		// no clients to broadcast to
		return;
	}
	uint32_t channel = 0;
	uint32_t flags = 0;
	if (type == DeliveryType::RELIABLE) {
		channel = RELIABLE_CHANNEL;
		flags = ENET_PACKET_FLAG_RELIABLE;
	} else {
		channel = UNRELIABLE_CHANNEL;
		flags = ENET_PACKET_FLAG_UNSEQUENCED;
	}
	// create the packet
	ENetPacket* p = enet_packet_create(
		&data[0],
		data.size(),
		flags);
	// send the packet to the peer
	enet_host_broadcast(host_, channel, p);
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
				// LOG_DEBUG("A packet of length "
				// 	<< event.packet->dataLength
				// 	<< " containing `"
				// 	<< event.packet->data
				// 	<< "` was received from client_"
				// 	<< event.peer->incomingPeerID);
				// add msg
				auto msg = Message::alloc(
					event.peer->incomingPeerID,
					MessageType::DATA,
					StreamBuffer::alloc(
						event.packet->data,
						event.packet->dataLength));
				msgs.push_back(msg);
				// destroy packet payload
				enet_packet_destroy(event.packet);

			} else if (event.type == ENET_EVENT_TYPE_CONNECT) {
				// client connected
				LOG_DEBUG("Client has connected from "
					<< addressToString(&event.peer->address)
					<< ", "
					<< numClients()
					<< " connected clients");
				// add msg
				auto msg = Message::alloc(
					event.peer->incomingPeerID,
					MessageType::CONNECT);
				msgs.push_back(msg);
				clients_[event.peer->incomingPeerID] = event.peer;

			} else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
				// client disconnected
				LOG_DEBUG("Client has disconnected from "
					<< addressToString(&event.peer->address)
					<< ", "
					<< numClients()
					<< " clients remaining");
				// add msg
				auto msg = Message::alloc(
					event.peer->incomingPeerID,
					MessageType::DISCONNECT);
				msgs.push_back(msg);
				clients_.erase(event.peer->incomingPeerID);
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
