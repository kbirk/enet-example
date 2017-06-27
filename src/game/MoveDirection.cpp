#include "game/MoveDirection.h"

#include "game/Idle.h"
#include "game/InputType.h"
#include "game/MoveTo.h"
#include "game/StateType.h"
#include "math/Math.h"
#include "time/Time.h"

const float32_t PLAYER_SPEED = 5.0;

MoveDirection::Shared MoveDirection::alloc(const Input::Shared& input)
{
    return std::make_shared<MoveDirection>(input);
}

MoveDirection::MoveDirection(const Input::Shared& input)
    : State(StateType::MOVE_DIRECTION)
    , timestamp_(Time::timestamp())
{
    if (input) {
        auto iter = input->find("direction");
        if (iter != input->end()) {
            auto dir = iter.value();
            direction_ = glm::vec3(dir[0], dir[1], dir[2]);
            direction_ = glm::normalize(direction_);
        }
    }
}

State::Shared MoveDirection::handleInput(const Input::Shared& input)
{
    switch (input->type()) {
    case InputType::MOVE_DIRECTION:
        return MoveDirection::alloc(input);

    case InputType::MOVE_TO:
        return MoveTo::alloc(input);

    case InputType::MOVE_STOP:
        return Idle::alloc();

        // case InputType::JUMP:
        //     return Jump::alloc(input);
    }
    return nullptr;
}

State::Shared MoveDirection::update(Player::Shared& player, Environment::Shared env, std::time_t dt)
{
    auto fdt = Time::toSeconds(dt);
    auto translation = direction_ * fdt * PLAYER_SPEED;
    auto a = -player->transform()->z();
    auto b = glm::normalize(translation);
    auto angle = Math::signedAngle(a, b, glm::vec3(0, 1, 0));
    auto nval = std::min(angle, angle * float32_t(fdt) * PLAYER_SPEED);
    player->transform()->rotateGlobal(nval, glm::vec3(0, 1, 0));
    player->moveAlong(translation, env);
    return nullptr;
}

StreamBuffer::Shared& MoveDirection::serialize(StreamBuffer::Shared& stream) const
{
    stream << timestamp_;
    stream << direction_;
    return stream;
}

StreamBuffer::Shared& MoveDirection::deserialize(StreamBuffer::Shared& stream)
{
    stream >> timestamp_;
    stream >> direction_;
    return stream;
}
