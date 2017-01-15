#pragma once

#include "Common.h"
#include "game/State.h"
//#include "game/Player.h"
#include "input/Input.h"

class Player;

class StateMachine {

	public:

		typedef std::shared_ptr<StateMachine> Shared;
		static Shared alloc();

		StateMachine();

		const State::Shared& current() const;

		void set(State::Shared);

		void handleInput(const Input::Shared&);
		void update(std::shared_ptr<Player>, std::time_t);

	private:

		// prevent copy-construction
		StateMachine(const StateMachine&);
		// prevent assignment
		StateMachine& operator= (const StateMachine&);

		State::Shared current_;

};
