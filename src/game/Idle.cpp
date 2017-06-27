#include "game/Idle.h"

#include "game/InputType.h"
#include "game/MoveDirection.h"
#include "game/MoveTo.h"
#include "game/StateType.h"

Idle::Shared Idle::alloc(const Input::Shared& input)
{
    return std::make_shared<Idle>(input);
}

Idle::Idle(const Input::Shared& input)
    : State(StateType::IDLE)
{
}

State::Shared Idle::handleInput(const Input::Shared& input)
{
    switch (input->type()) {
    case InputType::MOVE_DIRECTION:
        return MoveDirection::alloc(input);

    case InputType::MOVE_TO:
        return MoveTo::alloc(input);

        // case InputType::JUMP:
        //     return Jump::alloc(input);
    }
    return nullptr;
}

State::Shared Idle::update(Player::Shared& player, Environment::Shared env, std::time_t t)
{
    return nullptr;
}

StreamBuffer::Shared& Idle::serialize(StreamBuffer::Shared& stream) const
{
    return stream;
}

StreamBuffer::Shared& Idle::deserialize(StreamBuffer::Shared& stream)
{
    return stream;
}
