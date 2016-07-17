#include "net/Packet.h"

#include <cstring>

Packet::Shared Packet::alloc(const void* data, uint32_t numBytes) {
	return std::make_shared<Packet>(data, numBytes);
}

Packet::Packet(const void* data, uint32_t numBytes)
	: numBytes_(numBytes) {
	data_ = new uint8_t[numBytes_];
	memcpy(data_, data, numBytes_);
}

Packet::~Packet() {
	numBytes_ = 0;
	delete[] (uint8_t*)data_;
	data_ = nullptr;
}

const void* Packet::data() const {
	return data_;
}

uint32_t Packet::numBytes() const {
	return numBytes_;
}
