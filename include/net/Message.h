#pragma once

#include <string>
#include <vector>

enum class MessageType {
	CONNECT,
	DISCONNECT,
	DATA
};

struct Message {

	public:

		typedef std::shared_ptr<Message> Shared;
		static Shared alloc(MessageType);
		static Shared alloc(MessageType, const std::string&);

		Message(MessageType);
		Message(MessageType, const std::string&);

		MessageType type() const;
		const std::string& text() const;

	private:

		// prevent copy-construction
		Message(const Message&);
		// prevent assignment
		Message& operator= (const Message&);

		MessageType type_;
		std::string text_;

};
