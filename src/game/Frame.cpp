#include "game/Frame.h"

Frame::Shared Frame::alloc() {
	return std::make_shared<Frame>();
}

Frame::Frame()
	: timestamp_(timestamp()) {
}

void Frame::addPlayer(uint32_t id, Transform::Shared player) {
	players_[id] = player;
}

void Frame::removePlayer(uint32_t id) {
	players_.erase(id);
}

const std::map<uint32_t, Transform::Shared>& Frame::players() const {
	return players_;
}

std::time_t Frame::timestamp() const {
	return timestamp_;
}

void Frame::setTimestamp(std::time_t timestamp) {
	timestamp_ = timestamp;
}

StreamBuffer::Shared& operator<< (StreamBuffer::Shared& stream, const Frame::Shared& frame) {
	stream << frame->timestamp_; // timestamp
	stream << uint32_t(frame->players_.size()); // player count
	for (auto iter : frame->players_) {
		stream << iter.first; // id
		stream << iter.second; // player
	}
	return stream;
}

StreamBuffer::Shared& operator>> (StreamBuffer::Shared& stream, Frame::Shared& frame) {
	stream >> frame->timestamp_; // timestamp
	uint32_t size = 0;
	stream >> size; // player count
	for (auto i = 0; i < size; i++) {
		uint32_t id;
		stream >> id; // id
		auto player = Transform::alloc();
		stream >> player; // player
		frame->players_[id] = player;
	}
	return stream;
}

Frame::Shared interpolate(const Frame::Shared& a, const Frame::Shared& b, float32_t t) {

	// new interpolated frame
	auto frame = Frame::alloc();
	frame->setTimestamp(a->timestamp() + ((b->timestamp() - a->timestamp()) * t));

	// get all player ids in both frames
	std::set<uint32_t> ids;
	for (auto iter : a->players()) {
		ids.insert(iter.first);
	}
	for (auto iter : b->players()) {
		ids.insert(iter.first);
	}

	// interpolate new frame state
	for (auto id : ids) {
		auto pa = a->players().find(id);
		auto pb = b->players().find(id);

		auto aExists = pa != a->players().end();
		auto bExists = pb != b->players().end();

		if (!aExists && bExists) {
			// new player
			frame->addPlayer(id, pb->second);
			continue;
		}

		if (aExists && !bExists) {
			// player is gone
			continue;
		}

		// interpolate
		frame->addPlayer(id, interpolate(pa->second, pb->second, t));
	}

	return frame;
}
