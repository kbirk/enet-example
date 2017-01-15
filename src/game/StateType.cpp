#include "game/StateType.h"

#include "game/Idle.h"
#include "game/MoveDirection.h"

StreamBuffer::Shared& operator<< (StreamBuffer::Shared& stream, const StateMachine::Shared& machine) {
	auto state = machine->current();
	if (state) {
		stream << state->type();
		stream << state;
	} else {
		stream << uint8_t(StateType::NONE);
	}
	return stream;
}

StreamBuffer::Shared& operator>> (StreamBuffer::Shared& stream, StateMachine::Shared& machine) {
	// read type
	uint8_t type;
	stream >> type;
	// instantiate state based on type
	State::Shared state;
	switch (type) {
		case StateType::NONE:
			// no state, exit
			return stream;

		case StateType::IDLE:
			state = Idle::alloc();
			break;

		case StateType::MOVE_DIRECTION:
			state = MoveDirection::alloc();
			break;
	}
	stream >> state;
	machine->set(state);
	return stream;
}
