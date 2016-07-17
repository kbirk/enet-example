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
		static Shared alloc(const void*, uint32_t);

		Packet(const void*, uint32_t);
		~Packet();

		const void* data() const;
		uint32_t numBytes() const;

	private:

		// prevent copy-construction
		Packet(const Packet&);
		// prevent assignment
		Packet& operator= (const Packet&);

		void* data_;
		uint32_t numBytes_;
};
