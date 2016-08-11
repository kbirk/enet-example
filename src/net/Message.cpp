#include "net/Message.h"

Message::Shared Message::alloc(uint32_t id, MessageType type, StreamBuffer::Shared stream) {
	return std::make_shared<Message>(id, type, stream);
}

Message::Message(uint32_t id, MessageType type, StreamBuffer::Shared stream)
	: id_(id)
	, type_(type)
	, stream_(stream) {}

uint32_t Message::id() const {
	return id_;
}

MessageType Message::type() const {
	return type_;
}

StreamBuffer::Shared Message::stream() const {
	return stream_;
}
