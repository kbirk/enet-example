#pragma once

#include "Common.h"
#include "input/Input.h"
#include "game/State.h"
#include "game/StateMachine.h"
#include "math/Transform.h"
#include "serial/StreamBuffer.h"

class Player : public std::enable_shared_from_this<Player> {

	public:

		typedef std::shared_ptr<Player> Shared;
		static Shared alloc();
		static Shared alloc(Transform::Shared, State::Shared);

		Player();
		Player(Transform::Shared, State::Shared);

		void update(std::time_t);

		Transform::Shared transform();
		StateMachine::Shared state();

		friend StreamBuffer::Shared& operator<< (StreamBuffer::Shared&, const Player::Shared&);
		friend StreamBuffer::Shared& operator>> (StreamBuffer::Shared&, Player::Shared&);

	private:

		Transform::Shared transform_;
		StateMachine::Shared state_;
};

Player::Shared interpolate(const Player::Shared&, const Player::Shared&, float32_t);
