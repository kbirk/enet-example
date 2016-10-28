#include "Common.h"

#include "log/Log.h"

std::time_t timestamp() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();
}

void sleepMS(uint32_t ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
