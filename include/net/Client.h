#pragma once

#include "net/Message.h"

#include <enet/enet.h>

#include <iostream>
#include <map>
#include <memory>
#include <vector>

const uint32_t CONNECTION_TIMEOUT_MS = 5000;
const uint32_t RELIABLE_CHANNEL = 0;
const uint32_t UNRELIABLE_CHANNEL = 1;
const uint32_t NUM_CHANNELS = 2;

class Client {

	public:

		typedef std::shared_ptr<Client> Shared;
		static Shared alloc();

		Client();
		~Client();

		bool connect(const std::string&, uint32_t);
		bool disconnect();
		bool isConnected() const;

		void send(PacketType, const Packet::Shared&) const;
		std::vector<Message::Shared> poll();

	private:

		// prevent copy-construction
		Client(const Client&);
		// prevent assignment
		Client& operator= (const Client&);

		ENetHost* host_;

};
