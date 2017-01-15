#include "input/Input.h"

Input::Shared Input::alloc(uint8_t type) {
	return std::make_shared<Input>(type);
}

Input::Input(uint8_t type)
	: type_(type) {
}

uint8_t Input::type() const {
	return type_;
}

StreamBuffer::Shared& operator<< (StreamBuffer::Shared& stream, const Input::Shared& input) {
	stream << input->type();
	stream << input->dump();
	return stream;
}

StreamBuffer::Shared& operator>> (StreamBuffer::Shared& stream, Input::Shared& input) {
	// type
	stream >> input->type_;
	// args
	std::string str;
	stream >> str;
	std::stringstream ss(str);
	*input << ss;
	return stream;
}
