#pragma once

#include "serial/StreamBuffer.h"

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
		static Shared alloc(uint32_t, MessageType, StreamBuffer::Shared = nullptr);

		Message(uint32_t, MessageType, StreamBuffer::Shared = nullptr);

		uint32_t id() const;
		MessageType type() const;
		StreamBuffer::Shared stream() const;

	private:

		// prevent copy-construction
		Message(const Message&);
		// prevent assignment
		Message& operator= (const Message&);

		uint32_t id_;
		MessageType type_;
		StreamBuffer::Shared stream_;
};
