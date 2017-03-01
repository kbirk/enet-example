#include "enet/ENetClient.h"

#include "Common.h"
#include "log/Log.h"
#include "time/Time.h"

const uint8_t SERVER_ID = 0;
const std::time_t TIMEOUT_MS = 5000;
const std::time_t REQUEST_INTERVAL = Time::fromSeconds(1.0/60.0);

ENetClient::Shared ENetClient::alloc() {
	return std::make_shared<ENetClient>();
}

ENetClient::ENetClient()
	: host_(nullptr)
	, currentMsgId_(0) {
	// initialize enet
	// TODO: prevent this from being called multiple times
	if (enet_initialize() != 0) {
		LOG_ERROR("An error occurred while initializing ENet");
		return;
	}
	// create a host
	host_ = enet_host_create(
		nullptr, // create a client host
		1, // only allow 1 outgoing connection
		NUM_CHANNELS, // allow up TO N channels to be used
		0, // assume any amount of incoming bandwidth
		0); // assume any amount of outgoing bandwidth
	// check if creation was successful
	// NOTE: this only fails if malloc fails inside `enet_host_create`
	if (host_ == nullptr) {
		LOG_ERROR("An error occurred while trying to create an ENet client host");
	}
}

ENetClient::~ENetClient() {
	// disconnect if haven't already
	disconnect();
	// destroy the host
	// NOTE: safe to call when host is nullptr
	enet_host_destroy(host_);
	host_ = nullptr;
	// TODO: prevent this from being called multiple times
	enet_deinitialize();
}

bool ENetClient::connect(const std::string& host, uint32_t port) {
	if (isConnected()) {
		LOG_DEBUG("ENetClient is already connected to a server");
		return 0;
	}
	// set address to connect to
	ENetAddress address;
	enet_address_set_host(&address, host.c_str());
	address.port = port;
	// initiate the connection, allocating the two channels 0 and 1.
	server_ = enet_host_connect(host_, &address, NUM_CHANNELS, 0);
	if (server_ == nullptr) {
		LOG_ERROR("No available peers for initiating an ENet connection");
		return 1;
	}
	// attempt to connect to the peer (server)
	ENetEvent event;
	// wait N for connection to succeed
	// NOTE: we don't need to check / destroy packets because the server will be
	// unable to send the packets without first establishing a connection
	if (enet_host_service(host_, &event, TIMEOUT_MS) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT) {
		// connection successful
		LOG_DEBUG("Connection to `" << host << ":" << port << "` succeeded");
		return 0;
	}
	// failure to connect
	LOG_ERROR("Connection to `" << host << ":" << port << "` failed");
	enet_peer_reset(server_);
	server_ = nullptr;
	return 1;
}

bool ENetClient::disconnect() {
	if (!isConnected()) {
		return 0;
	}
	LOG_DEBUG("Disconnecting from server...");
	// attempt to gracefully disconnect
	enet_peer_disconnect(server_, 0);
	// wait for the disconnect to be acknowledged
	ENetEvent event;
	auto timestamp = Time::timestamp();
	bool success = false;
	while (true) {
		int32_t res = enet_host_service(host_, &event, 0);
		if (res > 0) {
			// event occured
			if (event.type == ENET_EVENT_TYPE_RECEIVE) {
				// throw away any received packets during disconnect
				enet_packet_destroy(event.packet);
			} else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
				// disconnect successful
				LOG_DEBUG("Disconnection from server successful");
				success = true;
				break;
			}
		} else if (res < 0) {
			// error occured
			LOG_ERROR("Encountered error while polling");
			break;
		}
		// check for timeout
		if (Time::timestamp() - timestamp > Time::fromMilliseconds(TIMEOUT_MS)) {
			break;
		}
	}
	if (!success) {
		// disconnect attempt didn't succeed yet, force close the connection
		LOG_ERROR("Disconnection was not acknowledged by server, shutdown forced");
		enet_peer_reset(server_);
	}
	server_ = nullptr;
	return !success;
}

bool ENetClient::isConnected() const {
	return host_->connectedPeers > 0;
}

void ENetClient::on(uint32_t id, RequestHandler handler) {
	handlers_[id] = handler;
}

void ENetClient::handleRequest(uint32_t requestId, StreamBuffer::Shared stream) const {
	auto iter = handlers_.find(requestId);
	if (iter != handlers_.end()) {
		auto handler = iter->second;
		auto res = handler(SERVER_ID, stream);
		sendResponse(requestId, res);
	}
}

void ENetClient::sendResponse(uint32_t requestId, StreamBuffer::Shared stream) const {
	if (!isConnected()) {
		LOG_DEBUG("ENetClient is not connected to any server");
		return;
	}
	auto msg = Message::alloc(
		++currentMsgId_, // id
		requestId, // request id
		MessageType::DATA_RESPONSE,
		stream);
	sendMessage(DeliveryType::RELIABLE, msg);
}

void ENetClient::sendMessage(DeliveryType type, Message::Shared msg) const {
	uint32_t channel = 0;
	uint32_t flags = 0;
	if (type == DeliveryType::RELIABLE) {
		channel = RELIABLE_CHANNEL;
		flags = ENET_PACKET_FLAG_RELIABLE;
	} else {
		channel = UNRELIABLE_CHANNEL;
		flags = ENET_PACKET_FLAG_UNSEQUENCED;
	}

	// get bytes
	auto data = msg->serialize();

	// create the packet
	ENetPacket* p = enet_packet_create(
		&data[0],
		data.size(),
		flags);

	// send the packet to the peer
	enet_peer_send(server_, channel, p);
	// flush / send the packet queue
	enet_host_flush(host_);
}

void ENetClient::send(DeliveryType type, StreamBuffer::Shared stream) const {
	if (!isConnected()) {
		LOG_DEBUG("ENetClient is not connected to any server");
		return;
	}
	auto msg = Message::alloc(
		++currentMsgId_,  // id
		MessageType::DATA,
		stream);
	sendMessage(type, msg);
}

void ENetClient::sendRequest(uint32_t requestId, StreamBuffer::Shared stream) const {
	auto msg = Message::alloc(
		++currentMsgId_,  // id
		requestId,  // request id
		MessageType::DATA_REQUEST,
		stream);
	sendMessage(DeliveryType::RELIABLE, msg);
}

Message::Shared ENetClient::request(uint32_t requestId, StreamBuffer::Shared stream) {
	if (!isConnected()) {
		LOG_DEBUG("ENetClient is not connected to any server");
		return nullptr;
	}
	// send request
	sendRequest(requestId, stream);
	// wait for response
	auto timestamp = Time::timestamp();
	while (true) {
		auto msgs = poll();
		for (auto msg : msgs) {
			if (msg->type() == MessageType::DATA_RESPONSE &&
				msg->requestId() == requestId) {
				return msg;
			}
			queue_.push_back(msg);
		}
		if (Time::timestamp() - timestamp > Time::fromMilliseconds(TIMEOUT_MS)) {
			break;
		}
		Time::sleep(REQUEST_INTERVAL);
	}
	return nullptr;
}

std::vector<Message::Shared> ENetClient::poll() {
	std::vector<Message::Shared> msgs;
	if (!isConnected()) {
		LOG_DEBUG("ENetClient is not connected to any server");
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

				// convert message to stream
				auto stream = StreamBuffer::alloc(
					event.packet->data,
					event.packet->dataLength);

				// deserialize message
				auto msg = Message::alloc(SERVER_ID);
				msg->deserialize(stream);
				msgs.push_back(msg);

				// handle requests
				if (msg->type() == MessageType::DATA_REQUEST) {
					handleRequest(msg->requestId(), msg->stream());
				}

				// destroy packet payload
				enet_packet_destroy(event.packet);

			} else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
				// server disconnected
				LOG_DEBUG("Connection to server has been lost");
				auto msg = Message::alloc(
					SERVER_ID,
					MessageType::DISCONNECT);
				msgs.push_back(msg);
				server_ = nullptr;
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
	if (!queue_.empty()) {
		msgs.insert(msgs.end(), queue_.begin(), queue_.end());
		queue_ = std::vector<Message::Shared>();
	}
	return msgs;
}
