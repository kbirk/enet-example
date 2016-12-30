#pragma once

#include "Common.h"
#include "log/Log.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <memory>

class StreamBuffer {

	public:

		typedef std::shared_ptr<StreamBuffer> Shared;
		static Shared alloc(size_t = 1024);
		static Shared alloc(const uint8_t*, size_t);

		explicit
		StreamBuffer(size_t = 1024);
		StreamBuffer(const uint8_t*, size_t);

		const std::vector<uint8_t>& buffer() const;
		void seekg(size_t);
		void seekp(size_t);
		size_t tellg() const;
		size_t tellp() const;
		size_t size() const;
		bool eof() const;

		void write(uint8_t);
		void write(uint16_t);
		void write(uint32_t);
		void write(uint64_t);
		void write(float32_t);
		void write(float64_t);
		void write(std::time_t);
		void write(const glm::vec3&);
		void write(const glm::quat&);

		void read(uint8_t&);
		void read(int8_t&);
		void read(uint16_t&);
		void read(int16_t&);
		void read(uint32_t&);
		void read(int32_t&);
		void read(uint64_t&);
		void read(int64_t&);
		void read(float32_t&);
		void read(float64_t&);
		void read(std::time_t&);
		void read(glm::vec3&);
		void read(glm::quat&);

	private:

		// prevent copy-construction
		StreamBuffer(const StreamBuffer&);
		// prevent assignment
		StreamBuffer& operator= (const StreamBuffer&);

		size_t gpos_;
		size_t ppos_;
		std::vector<uint8_t> buffer_;
};

template<typename T>
StreamBuffer::Shared& operator<< (StreamBuffer::Shared& stream, const T& data) {
	stream->write(data);
	return stream;
}

template<typename T>
StreamBuffer::Shared& operator>> (StreamBuffer::Shared& stream, T& data) {
	stream->read(data);
	return stream;
}
