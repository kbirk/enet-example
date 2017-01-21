#pragma once

#include "Common.h"
#include "game/Player.h"
#include "game/State.h"
#include "input/Input.h"
#include "serial/StreamBuffer.h"

class MoveTo : public State {

	public:

		typedef std::shared_ptr<MoveTo> Shared;
		static Shared alloc(const Input::Shared& = nullptr);

		explicit
		MoveTo(const Input::Shared& = nullptr);

		State::Shared handleInput(const Input::Shared&);
		State::Shared update(Player::Shared&, std::time_t);

	protected:

		StreamBuffer::Shared& serialize(StreamBuffer::Shared&) const;
		StreamBuffer::Shared& deserialize(StreamBuffer::Shared&);

	private:

		std::time_t timestamp_;
		glm::vec3 position_;
};
