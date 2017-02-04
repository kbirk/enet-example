#include "game/Player.h"

#include "game/Idle.h"
#include "game/MoveDirection.h"
#include "game/InputType.h"
#include "game/StateType.h"

Player::Shared Player::alloc(uint32_t id) {
	return std::make_shared<Player>(id);
}

Player::Shared Player::alloc(uint32_t id, Transform::Shared transform, State::Shared state) {
	return std::make_shared<Player>(id, transform, state);
}

Player::Player(uint32_t id)
	: Player(id, Transform::alloc(), Idle::alloc(nullptr)) {
}

Player::Player(uint32_t id, Transform::Shared transform, State::Shared state)
 	: id_(id)
	, transform_(transform) {
	state_ = StateMachine::alloc();
	state_->set(state);
}

void Player::update(Environment::Shared env, std::time_t dt) {
	state_->update(shared_from_this(), env, dt);
}

void Player::moveAlong(glm::vec3 translation, Environment::Shared env) {
	auto origin = transform_->translation() + translation;
	auto intersection = env->intersect(
		glm::vec3(0, -1, 0),
		origin,
		false,-
		false);
	if (intersection.hit) {
		transform_->setTranslation(intersection.position);
	}
}

Transform::Shared Player::transform() {
	return transform_;
}

StateMachine::Shared Player::state() {
	return state_;
}

uint32_t Player::id() const {
	return id_;
}

StreamBuffer::Shared& operator<< (StreamBuffer::Shared& stream, const Player::Shared& player) {
	stream << player->id_;
	stream << player->transform_;
	stream << player->state_;
	return stream;
}

StreamBuffer::Shared& operator>> (StreamBuffer::Shared& stream, Player::Shared& player) {
	stream >> player->id_;
	stream >> player->transform_;
	stream >> player->state_;
	return stream;
}

Player::Shared interpolate(const Player::Shared& a, const Player::Shared& b, float32_t t) {
	auto transform = interpolate(a->transform(), b->transform(), t);
	auto state = b->state()->current();
	return Player::alloc(a->id(), transform, state);
}
