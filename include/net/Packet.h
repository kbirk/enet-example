#pragma once

#include <memory>
#include <string>
#include <vector>

enum class PacketType {
	RELIABLE,
	UNRELIABLE
};

class Packet {

	public:

		typedef std::shared_ptr<Packet> Shared;
		static Shared alloc(const uint8_t*, uint32_t);

		Packet(const uint8_t*, uint32_t);
		~Packet();

		const uint8_t* data() const;
		uint32_t numBytes() const;

	private:

		// prevent copy-construction
		Packet(const Packet&);
		// prevent assignment
		Packet& operator= (const Packet&);

		uint8_t* data_;
		uint32_t numBytes_;
};
