#include "game/MoveDirection.h"

#include "game/InputType.h"
#include "game/Idle.h"
#include "game/MoveTo.h"
#include "game/StateType.h"
#include "time/Time.h"

const float32_t PLAYER_SPEED = 5;

MoveDirection::Shared MoveDirection::alloc(const Input::Shared& input) {
	return std::make_shared<MoveDirection>(input);
}

MoveDirection::MoveDirection(const Input::Shared& input)
 	: State(StateType::MOVE_DIRECTION)
	, timestamp_(Time::timestamp()) {
	if (input) {
		auto iter = input->find("direction");
		if (iter != input->end()) {
			auto dir = iter.value();
			direction_ = glm::vec3(dir[0], dir[1], dir[2]);
		}
	}
}

State::Shared MoveDirection::handleInput(const Input::Shared& input) {
	switch (input->type()) {
		case InputType::MOVE_DIRECTION:
			return MoveDirection::alloc(input);

		case InputType::MOVE_TO:
			return MoveTo::alloc(input);

		case InputType::MOVE_STOP:
			return Idle::alloc(input);

		// case InputType::JUMP:
		// 	return Jump::alloc(input);
	}
	return nullptr;
}

State::Shared MoveDirection::update(Player::Shared& player, std::time_t dt) {
	auto fdt = dt / float64_t(Time::seconds(1));
	auto translation = direction_ * fdt * PLAYER_SPEED;
	player->transform()->translateGlobal(translation);
	return nullptr;
}

StreamBuffer::Shared& MoveDirection::serialize(StreamBuffer::Shared& stream) const {
	stream << timestamp_;
	stream << direction_;
	return stream;
}

StreamBuffer::Shared& MoveDirection::deserialize(StreamBuffer::Shared& stream) {
	stream >> timestamp_;
	stream >> direction_;
	return stream;
}
