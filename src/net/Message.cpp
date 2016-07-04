#include "net/Message.h"

Message::Shared Message::alloc(MessageType type) {
	return std::make_shared<Message>(type);
}

Message::Shared Message::alloc(MessageType type, const std::string& text) {
	return std::make_shared<Message>(type, text);
}

Message::Message(MessageType type)
	: type_(type) {}

Message::Message(MessageType type, const std::string& text)
	: type_(type)
	, text_(text) {}

MessageType Message::type() const {
	return type_;
}

const std::string& Message::text() const {
	return text_;
}
