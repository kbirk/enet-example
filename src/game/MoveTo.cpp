#include "game/MoveTo.h"

#include "game/InputType.h"
#include "game/Idle.h"
#include "game/MoveDirection.h"
#include "game/StateType.h"
#include "time/Time.h"

const float32_t PLAYER_SPEED = 5;

MoveTo::Shared MoveTo::alloc(const Input::Shared& input) {
	return std::make_shared<MoveTo>(input);
}

MoveTo::MoveTo(const Input::Shared& input)
 	: State(StateType::MOVE_TO)
	, timestamp_(Time::timestamp()) {
	if (input) {
		auto iter = input->find("position");
		if (iter != input->end()) {
			auto pos = iter.value();
			position_ = glm::vec3(pos[0], pos[1], pos[2]);
		}
	}
}

State::Shared MoveTo::handleInput(const Input::Shared& input) {
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

State::Shared MoveTo::update(Player::Shared& player, std::time_t dt) {
	auto diff = position_ - player->transform()->translation();
	auto dist = glm::length(diff);
	auto fdt = dt / float32_t(Time::seconds(1));
	dist = std::min(fdt * PLAYER_SPEED, dist);
	if (dist < 0.000001) {
		return Idle::alloc(nullptr);
	}
	auto direction = glm::normalize(diff);
	auto translation = direction * dist;
	player->transform()->translateGlobal(translation);
	return nullptr;
}

StreamBuffer::Shared& MoveTo::serialize(StreamBuffer::Shared& stream) const {
	stream << timestamp_;
	stream << position_;
	return stream;
}

StreamBuffer::Shared& MoveTo::deserialize(StreamBuffer::Shared& stream) {
	stream >> timestamp_;
	stream >> position_;
	return stream;
}
