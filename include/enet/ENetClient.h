#pragma once

#include "net/DeliveryType.h"
#include "net/Message.h"
#include "net/Client.h"

#include <enet/enet.h>

#include <iostream>
#include <map>
#include <memory>
#include <vector>

class ENetClient : public Client {

	public:

		typedef std::shared_ptr<ENetClient> Shared;
		static Shared alloc();

		ENetClient();
		~ENetClient();

		bool connect(const std::string&, uint32_t);
		bool disconnect();
		bool isConnected() const;

		void send(DeliveryType, StreamBuffer::Shared) const;

		Message::Shared request(uint32_t, StreamBuffer::Shared);

		std::vector<Message::Shared> poll();

		void on(uint32_t, RequestHandler);

	private:

		void sendMessage(DeliveryType type, Message::Shared msg) const;
		void sendRequest(uint32_t responseId, StreamBuffer::Shared stream) const;
		void sendResponse(uint32_t requestId, StreamBuffer::Shared stream) const;
		void handleRequest(uint32_t, StreamBuffer::Shared stream) const;

		// prevent copy-construction
		ENetClient(const ENetClient&);
		// prevent assignment
		ENetClient& operator= (const ENetClient&);

		ENetHost* host_;
		ENetPeer* server_;
		std::vector<Message::Shared> queue_;
		std::map<uint32_t, RequestHandler> handlers_;
		mutable uint32_t currentMsgId_;

};
