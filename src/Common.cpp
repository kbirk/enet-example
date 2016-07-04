#include "Common.h"

#include "Log.h"

std::time_t timestamp() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();
}

void sleepMS(uint32_t ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

std::string addressToString(const ENetAddress* address) {
	uint8_t a = (uint8_t)(address->host);
	uint8_t b = (uint8_t)(address->host >> 8);
	uint8_t c = (uint8_t)(address->host >> 16);
	uint8_t d = (uint8_t)(address->host >> 24);
	return std::to_string(a) + "." +
		std::to_string(b) + "." +
		std::to_string(c) + "." +
		std::to_string(d) + ":" +
		std::to_string(address->port);
}
