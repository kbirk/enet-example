#include "game/MoveTo.h"

#include "game/InputType.h"
#include "game/Idle.h"
#include "game/MoveDirection.h"
#include "game/StateType.h"
#include "math/Common.h"
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
			return Idle::alloc();

		// case InputType::JUMP:
		// 	return Jump::alloc(input);
	}
	return nullptr;
}

State::Shared MoveTo::update(Player::Shared& player, Environment::Shared env, std::time_t dt) {
	auto diff = position_ - player->transform()->translation();
	auto dist = glm::length(diff);
	auto fdt = Time::toSeconds(dt);
	dist = std::min(float32_t(fdt) * PLAYER_SPEED, dist);
	if (dist < M_EPSILON) {
		return Idle::alloc(nullptr);
	}
	auto direction = glm::normalize(diff);
	auto translation = direction * dist;

	auto src = -player->transform()->z();
	auto dst = glm::normalize(translation);

	auto angle = Math::signedAngle(src, dst, glm::vec3(0, 1, 0));
	auto nval = std::min(angle, angle * dist);

	player->transform()->rotateGlobal(nval, glm::vec3(0, 1, 0));
	// player->transform()->rotateLocal(-dist, glm::vec3(1, 0, 0));

	player->moveAlong(translation, env);

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
