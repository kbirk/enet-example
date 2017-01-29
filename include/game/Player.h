#pragma once

#include "Common.h"
#include "input/Input.h"
#include "game/Environment.h"
#include "game/State.h"
#include "game/StateMachine.h"
#include "math/Transform.h"
#include "serial/StreamBuffer.h"

class Player : public std::enable_shared_from_this<Player> {

	public:

		typedef std::shared_ptr<Player> Shared;
		static Shared alloc(uint32_t);
		static Shared alloc(uint32_t, Transform::Shared, State::Shared);

		Player(uint32_t);
		Player(uint32_t, Transform::Shared, State::Shared);

		uint32_t id() const;
		void update(Environment::Shared, std::time_t);

		void moveAlong(glm::vec3 translation, Environment::Shared);

		Transform::Shared transform();
		StateMachine::Shared state();

		friend StreamBuffer::Shared& operator<< (StreamBuffer::Shared&, const Player::Shared&);
		friend StreamBuffer::Shared& operator>> (StreamBuffer::Shared&, Player::Shared&);

	private:

		// prevent copy-construction
		Player(const Player&);
		// prevent assignment
		Player& operator= (const Player&);

		uint32_t id_;
		Transform::Shared transform_;
		StateMachine::Shared state_;
};

Player::Shared interpolate(const Player::Shared&, const Player::Shared&, float32_t);
