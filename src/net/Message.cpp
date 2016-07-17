#include "net/Message.h"

Message::Shared Message::alloc(MessageType type, Packet::Shared packet) {
	return std::make_shared<Message>(type, packet);
}

Message::Message(MessageType type, Packet::Shared packet)
	: type_(type)
	, packet_(packet) {}

MessageType Message::type() const {
	return type_;
}

Packet::Shared Message::packet() const {
	return packet_;
}
