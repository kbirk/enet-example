#pragma once

#include "Common.h"
#include "game/Environment.h"
#include "game/Player.h"
#include "game/State.h"
#include "input/Input.h"
#include "serial/StreamBuffer.h"

class Idle : public State {

public:
    typedef std::shared_ptr<Idle> Shared;
    static Shared alloc(const Input::Shared& = nullptr);

    explicit Idle(const Input::Shared& = nullptr);

    State::Shared handleInput(const Input::Shared&);
    State::Shared update(Player::Shared&, Environment::Shared, std::time_t);

protected:
    StreamBuffer::Shared& serialize(StreamBuffer::Shared&) const;
    StreamBuffer::Shared& deserialize(StreamBuffer::Shared&);
};
