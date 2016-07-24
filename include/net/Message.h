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
		static Shared alloc(uint32_t, MessageType, Packet::Shared = nullptr);

		Message(uint32_t, MessageType, Packet::Shared = nullptr);

		uint32_t id() const;
		MessageType type() const;
		Packet::Shared packet() const;

	private:

		// prevent copy-construction
		Message(const Message&);
		// prevent assignment
		Message& operator= (const Message&);

		uint32_t id_;
		MessageType type_;
		Packet::Shared packet_;
};
