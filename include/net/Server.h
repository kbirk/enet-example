#pragma once

#include "Common.h"
#include "net/DeliveryType.h"
#include "net/Message.h"
#include "serial/StreamBuffer.h"

#include <memory>
#include <vector>

class Server {

	public:

		typedef std::shared_ptr<Server> Shared;

		Server();
		virtual ~Server();

		virtual bool start(uint32_t) = 0;
		virtual bool stop() = 0;
		virtual bool isRunning() const = 0;

		virtual uint32_t numClients() const = 0;

		virtual void send(uint32_t, DeliveryType, StreamBuffer::Shared) const = 0;
		virtual void broadcast(DeliveryType, StreamBuffer::Shared) const = 0;
		virtual std::vector<Message::Shared> poll() = 0;

		virtual void on(uint32_t, RequestHandler) = 0;

	private:

		// prevent copy-construction
		Server(const Server&);
		// prevent assignment
		Server& operator= (const Server&);
};
