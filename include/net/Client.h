#pragma once

#include "Common.h"
#include "net/DeliveryType.h"
#include "net/Message.h"
#include "serial/StreamBuffer.h"

#include <memory>
#include <vector>

class Client {

public:
    typedef std::shared_ptr<Client> Shared;

    Client();
    virtual ~Client();

    virtual bool connect(const std::string&, uint32_t) = 0;
    virtual bool disconnect() = 0;
    virtual bool isConnected() const = 0;

    virtual void send(DeliveryType, StreamBuffer::Shared) const = 0;
    virtual Message::Shared request(uint32_t, StreamBuffer::Shared) = 0;
    virtual std::vector<Message::Shared> poll() = 0;

    virtual void on(uint32_t, RequestHandler) = 0;

private:
    // prevent copy-construction
    Client(const Client&);
    // prevent assignment
    Client& operator=(const Client&);
};
