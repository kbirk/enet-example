#pragma once

#include "net/DeliveryType.h"
#include "net/Message.h"

#include <enet/enet.h>

#include <iostream>
#include <map>
#include <memory>
#include <vector>

class Server {

	public:

		typedef std::shared_ptr<Server> Shared;
		static Shared alloc();

		Server();
		~Server();

		bool start(uint32_t);
		bool stop();
		bool isRunning() const;

		uint32_t numClients() const;

		void send(uint32_t, DeliveryType, const std::vector<uint8_t>&) const;
		void broadcast(DeliveryType, const std::vector<uint8_t>&) const;
		std::vector<Message::Shared> poll();

	private:

		// prevent copy-construction
		Server(const Server&);
		// prevent assignment
		Server& operator= (const Server&);

		ENetHost* host_;
		// NOTE: ENet allocates all peers at once and doesn't shuffle them,
		// which leads to non-contiguous connected peers. This map
		// will make it easier to manage them by id
		std::map<uint32_t, ENetPeer*> clients_;
};
