#pragma once

#include "net/Packet.h"

#include <memory>
#include <string>
#include <vector>

enum class MessageType {
	CONNECT,
	DISCONNECT,
	DATA
};

class Message {

	public:

		typedef std::shared_ptr<Message> Shared;
		static Shared alloc(MessageType, Packet::Shared = nullptr);

		Message(MessageType, Packet::Shared = nullptr);

		MessageType type() const;
		Packet::Shared packet() const;

	private:

		// prevent copy-construction
		Message(const Message&);
		// prevent assignment
		Message& operator= (const Message&);

		MessageType type_;
		Packet::Shared packet_;
};
