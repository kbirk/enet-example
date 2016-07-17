#pragma once

#include "net/Message.h"

#include <enet/enet.h>

#include <iostream>
#include <map>
#include <memory>
#include <vector>

const uint32_t CONNECTION_TIMEOUT_MS = 5000;
const uint32_t MAX_CONNECTIONS = 64;
const uint32_t RELIABLE_CHANNEL = 0;
const uint32_t UNRELIABLE_CHANNEL = 1;
const uint32_t NUM_CHANNELS = 2;

class Server {

	public:

		typedef std::shared_ptr<Server> Shared;
		static Shared alloc();

		Server();
		~Server();

		bool start(uint32_t);
		bool stop();

		uint32_t numClients() const;

		void broadcast(PacketType, const Packet::Shared&) const;
		std::vector<Message::Shared> poll();

	private:

		// prevent copy-construction
		Server(const Server&);
		// prevent assignment
		Server& operator= (const Server&);

		void addClient(ENetPeer*);
		void removeClient(ENetPeer*);

		ENetHost* host_;
		std::map<uint32_t, ENetPeer*> clients_;
};
