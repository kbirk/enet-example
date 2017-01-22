#include "game/Player.h"

#include "game/Idle.h"
#include "game/MoveDirection.h"
#include "game/InputType.h"
#include "game/StateType.h"

Player::Shared Player::alloc() {
	return std::make_shared<Player>();
}

Player::Shared Player::alloc(Transform::Shared transform, State::Shared state) {
	return std::make_shared<Player>(transform, state);
}

Player::Player()
	: Player(Transform::alloc(), Idle::alloc(nullptr)) {
}

Player::Player(Transform::Shared transform, State::Shared state)
 	: transform_(transform) {
	state_ = StateMachine::alloc();
	state_->set(state);
}

void Player::update(std::time_t dt) {
	state_->update(shared_from_this(), dt);
}

Transform::Shared Player::transform() {
	return transform_;
}

StateMachine::Shared Player::state() {
	return state_;
}

StreamBuffer::Shared& operator<< (StreamBuffer::Shared& stream, const Player::Shared& player) {
	stream << player->transform_;
	stream << player->state_;
	return stream;
}

StreamBuffer::Shared& operator>> (StreamBuffer::Shared& stream, Player::Shared& player) {
	stream >> player->transform_;
	stream >> player->state_;
	return stream;
}

Player::Shared interpolate(const Player::Shared& a, const Player::Shared& b, float32_t t) {
	auto transform = interpolate(a->transform(), b->transform(), t);
	auto state = b->state()->current();
	return Player::alloc(transform, state);
}
