#pragma once

#include "serial/StreamBuffer.h"

#include <memory>
#include <string>
#include <vector>

typedef std::function<StreamBuffer::Shared(uint32_t, StreamBuffer::Shared)> RequestHandler;

namespace MessageType {
enum Types {
    CONNECT,
    DISCONNECT,
    DATA,
    DATA_REQUEST,
    DATA_RESPONSE
};
}

class Message {

public:
    typedef std::shared_ptr<Message> Shared;
    static Shared alloc(uint32_t id, uint8_t, StreamBuffer::Shared); // data
    static Shared alloc(uint32_t id, uint32_t requestId, uint8_t, StreamBuffer::Shared); // request / response
    static Shared alloc(uint32_t peerId, uint8_t); // connect / disconnect
    static Shared alloc(uint32_t peerId); // deserialize from

    Message(uint32_t id, uint8_t, StreamBuffer::Shared); // data
    Message(uint32_t id, uint32_t requestId, uint8_t, StreamBuffer::Shared); // request / response
    Message(uint32_t peerId, uint8_t); // connect / disconnect
    Message(uint32_t peerId); // deserialize from

    uint32_t peerId() const;
    uint32_t id() const;
    uint32_t requestId() const;
    uint8_t type() const;
    StreamBuffer::Shared stream() const;

    std::vector<uint8_t> serialize() const;
    void deserialize(StreamBuffer::Shared);

private:
    // prevent copy-construction
    Message(const Message&);
    // prevent assignment
    Message& operator=(const Message&);

    uint32_t id_;
    uint32_t peerId_;
    uint32_t requestId_;
    uint8_t type_;
    StreamBuffer::Shared stream_;
};
