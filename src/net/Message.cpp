#include "net/Message.h"

Message::Shared Message::alloc(uint32_t id, MessageType type, Packet::Shared packet) {
	return std::make_shared<Message>(id, type, packet);
}

Message::Message(uint32_t id, MessageType type, Packet::Shared packet)
	: id_(id)
	, type_(type)
	, packet_(packet) {}

uint32_t Message::id() const {
	return id_;
}

MessageType Message::type() const {
	return type_;
}

Packet::Shared Message::packet() const {
	return packet_;
}
