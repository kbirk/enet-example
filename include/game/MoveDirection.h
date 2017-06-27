#pragma once

#include "Common.h"
#include "game/Environment.h"
#include "game/Player.h"
#include "game/State.h"
#include "input/Input.h"
#include "serial/StreamBuffer.h"

class MoveDirection : public State {

public:
    typedef std::shared_ptr<MoveDirection> Shared;
    static Shared alloc(const Input::Shared& = nullptr);

    explicit MoveDirection(const Input::Shared& = nullptr);

    State::Shared handleInput(const Input::Shared&);
    State::Shared update(Player::Shared&, Environment::Shared, std::time_t);

protected:
    StreamBuffer::Shared& serialize(StreamBuffer::Shared&) const;
    StreamBuffer::Shared& deserialize(StreamBuffer::Shared&);

private:
    std::time_t timestamp_;
    glm::vec3 direction_;
};
