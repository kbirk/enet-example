#include "game/StateMachine.h"

#include "game/InputType.h"
#include "game/MoveDirection.h"

StateMachine::Shared StateMachine::alloc() {
	return std::make_shared<StateMachine>();
}

StateMachine::StateMachine() {
}

const State::Shared& StateMachine::current() const {
	return current_;
}

void StateMachine::set(State::Shared next) {
	// update current state
	current_ = next;
}

void StateMachine::handleInput(const Input::Shared& input) {
	if (current_) {
		auto next = current_->handleInput(input);
		if (next) {
			set(next);
		}
	}
}

void StateMachine::update(std::shared_ptr<Player> player, Environment::Shared env, std::time_t time) {
	if (current_) {
		auto next = current_->update(player, env, time);
		if (next) {
			set(next);
		}
	}
}
