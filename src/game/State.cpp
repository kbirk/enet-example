#include "game/State.h"

State::State(uint8_t type)
	: type_(type) {
}

State::~State() {
}

uint8_t State::type() const {
	return type_;
}

StreamBuffer::Shared& operator<< (StreamBuffer::Shared& stream, const State::Shared& state) {
	state->serialize(stream);
	return stream;
}

StreamBuffer::Shared& operator>> (StreamBuffer::Shared& stream, State::Shared& state) {
	state->deserialize(stream);
	return stream;
}
