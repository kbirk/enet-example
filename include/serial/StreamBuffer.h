#pragma once

#include "Common.h"
#include "log/Log.h"
#include "render/Transform.h"
#include "protocol/Command.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <memory>

class StreamBuffer {

	public:

		explicit
		StreamBuffer(size_t = 1024);
		StreamBuffer(const uint8_t*, size_t);

		const std::vector<uint8_t>& buffer() const;
		void seekg(size_t);
		void seekp(size_t);
		size_t tellg() const;
		size_t tellp() const;

		bool eof() const;

		StreamBuffer& operator<< (uint8_t);
		StreamBuffer& operator<< (uint32_t);
		StreamBuffer& operator<< (float32_t);
		StreamBuffer& operator<< (const glm::vec3&);
		StreamBuffer& operator<< (const glm::quat&);
		StreamBuffer& operator<< (const Transform::Shared&);
		StreamBuffer& operator<< (const Command&);

		StreamBuffer& operator>> (uint8_t&);
		StreamBuffer& operator>> (uint32_t&);
		StreamBuffer& operator>> (float32_t&);
		StreamBuffer& operator>> (glm::vec3&);
		StreamBuffer& operator>> (glm::quat&);
		StreamBuffer& operator>> (Transform::Shared&);
		StreamBuffer& operator>> (Command&);

	private:

		size_t gpos_;
		size_t ppos_;
		std::vector<uint8_t> buffer_;
};
