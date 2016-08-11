#include "serial/StreamBuffer.h"

#include "serial/Serialization.h"

StreamBuffer::Shared StreamBuffer::alloc(size_t numBytes) {
	return std::make_shared<StreamBuffer>(numBytes);
}

StreamBuffer::Shared StreamBuffer::alloc(const uint8_t* data, size_t numBytes) {
	return std::make_shared<StreamBuffer>(data, numBytes);
}

StreamBuffer::StreamBuffer(size_t numBytes)
	: gpos_(0)
	, ppos_(0) {
	buffer_.reserve(numBytes);
}

StreamBuffer::StreamBuffer(const uint8_t* data, size_t numBytes)
	: gpos_(0)
	, ppos_(0) {
	buffer_.reserve(numBytes);
	buffer_.assign(data, data + numBytes);
}

const std::vector<uint8_t>& StreamBuffer::buffer() const {
	return buffer_;
}

void StreamBuffer::seekg(size_t pos) {
	gpos_ = pos;
}

void StreamBuffer::seekp(size_t pos) {
	ppos_ = pos;
}

size_t StreamBuffer::tellg() const {
	return gpos_;
}

size_t StreamBuffer::tellp() const {
	return ppos_;
}

size_t StreamBuffer::size() const {
	return buffer_.size();
}

bool StreamBuffer::eof() const {
	return gpos_ >= buffer_.size();
}

void StreamBuffer::write(uint8_t data) {
	buffer_.push_back(data);
	ppos_++;
}

void StreamBuffer::write(uint32_t data) {
	std::vector<uint8_t> buff(sizeof(data), 0);
	std::memcpy(&buff[0], &data, sizeof(data));
	buffer_.insert(buffer_.end(), buff.begin(), buff.end());
	ppos_ += sizeof(data);
}

void StreamBuffer::write(float32_t data) {
	write(pack754_32(data));
}

void StreamBuffer::write(const glm::vec3& data) {
	write(data.x);
	write(data.y);
	write(data.z);
}

void StreamBuffer::write(const glm::quat& data) {
	write(data.x);
	write(data.y);
	write(data.z);
	write(data.w);
}

void StreamBuffer::read(uint8_t& data) {
	std::memcpy(&data, &buffer_[gpos_], sizeof(data));
	gpos_++;
}

void StreamBuffer::read(uint32_t& data) {
	std::memcpy(&data, &buffer_[gpos_], sizeof(data));
	gpos_ += sizeof(data);
}

void StreamBuffer::read(float32_t& data) {
	uint32_t packed = 0;
	read(packed);
	data = unpack754_32(packed);
}

void StreamBuffer::read(glm::vec3& data) {
	read(data.x);
	read(data.y);
	read(data.z);
}

void StreamBuffer::read(glm::quat& data) {
	read(data.x);
	read(data.y);
	read(data.z);
	read(data.w);
}
