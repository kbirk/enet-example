#pragma once

#include "Common.h"
#include "math/Transform.h"
#include "serial/StreamBuffer.h"

#include <memory>
#include <map>
#include <set>

class Frame {

	public:

		typedef std::shared_ptr<Frame> Shared;
		static Shared alloc();

		Frame();

		void addPlayer(uint32_t, Transform::Shared);
		void removePlayer(uint32_t);
		const std::map<uint32_t, Transform::Shared>& players() const;

		void setTimestamp(std::time_t);
		std::time_t timestamp() const;

		friend StreamBuffer::Shared& operator<< (StreamBuffer::Shared&, const Frame::Shared&);
		friend StreamBuffer::Shared& operator>> (StreamBuffer::Shared&, Frame::Shared&);

	private:

		// prevent copy-construction
		Frame(const Frame&);
		// prevent assignment
		Frame& operator= (const Frame&);

		std::map<uint32_t, Transform::Shared> players_;
		std::time_t timestamp_;
};

Frame::Shared interpolate(const Frame::Shared&, const Frame::Shared&, float32_t);
