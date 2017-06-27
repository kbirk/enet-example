#include "net/Message.h"

Message::Shared Message::alloc(uint32_t id, uint8_t type, StreamBuffer::Shared stream)
{
    return std::make_shared<Message>(id, type, stream);
}

Message::Shared Message::alloc(uint32_t id, uint32_t requestId, uint8_t type, StreamBuffer::Shared stream)
{
    return std::make_shared<Message>(id, requestId, type, stream);
}

Message::Shared Message::alloc(uint32_t peerId, uint8_t type)
{
    return std::make_shared<Message>(peerId, type);
}

Message::Shared Message::alloc(uint32_t peerId)
{
    return std::make_shared<Message>(peerId);
}

Message::Message(uint32_t id, uint8_t type, StreamBuffer::Shared stream)
    : id_(id)
    , peerId_(0)
    , requestId_(0)
    , type_(type)
    , stream_(stream)
{
}

Message::Message(uint32_t id, uint32_t requestId, uint8_t type, StreamBuffer::Shared stream)
    : id_(id)
    , peerId_(0)
    , requestId_(requestId)
    , type_(type)
    , stream_(stream)
{
}

Message::Message(uint32_t peerId, uint8_t type)
    : id_(0)
    , peerId_(peerId)
    , requestId_(0)
    , type_(type)
{
}

Message::Message(uint32_t peerId)
    : id_(0)
    , peerId_(peerId)
    , requestId_(0)
    , type_(0)
{
}

uint32_t Message::id() const
{
    return id_;
}

uint32_t Message::requestId() const
{
    return requestId_;
}

uint32_t Message::peerId() const
{
    return peerId_;
}

uint8_t Message::type() const
{
    return type_;
}

StreamBuffer::Shared Message::stream() const
{
    return stream_;
}

std::vector<uint8_t> Message::serialize() const
{
    auto header = StreamBuffer::alloc();
    header << id_;
    header << requestId_;
    header << type_;
    return merge(header, stream_)->buffer();
}

void Message::deserialize(StreamBuffer::Shared stream)
{
    stream >> id_;
    stream >> requestId_;
    stream >> type_;
    stream_ = stream;
}
