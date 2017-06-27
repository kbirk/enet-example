#pragma once

#include "Common.h"
#include "serial/StreamBuffer.h"

#include <memory>

#include "json.hpp"

class Input : public nlohmann::json {

public:
    typedef std::shared_ptr<Input> Shared;
    static Shared alloc(uint8_t);

    explicit Input(uint8_t);

    uint8_t type() const;

    friend StreamBuffer::Shared& operator<<(StreamBuffer::Shared&, const Input::Shared&);
    friend StreamBuffer::Shared& operator>>(StreamBuffer::Shared&, Input::Shared&);

private:
    // prevent copy-construction
    Input(const Input&);
    // prevent assignment
    Input& operator=(const Input&);

    uint8_t type_;
};
