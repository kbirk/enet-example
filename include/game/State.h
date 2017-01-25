#pragma once

#include "Common.h"
#include "input/Input.h"
#include "game/Environment.h"
#include "serial/StreamBuffer.h"

#include <memory>

class Player;

class State {

	public:

		typedef std::shared_ptr<State> Shared;

		explicit
		State(uint8_t);

		virtual ~State();

		uint8_t type() const;

		virtual State::Shared handleInput(const Input::Shared&) = 0;
		virtual State::Shared update(
			std::shared_ptr<Player>&,
			Environment::Shared,
			std::time_t) = 0;

		friend StreamBuffer::Shared& operator<< (StreamBuffer::Shared&, const State::Shared&);
		friend StreamBuffer::Shared& operator>> (StreamBuffer::Shared&, State::Shared&);

	protected:

		virtual StreamBuffer::Shared& serialize(StreamBuffer::Shared&) const = 0;
		virtual StreamBuffer::Shared& deserialize(StreamBuffer::Shared&) = 0;

	private:

		// prevent copy-construction
		State(const State&);
		// prevent assignment
		State& operator= (const State&);

		uint8_t type_;
};
