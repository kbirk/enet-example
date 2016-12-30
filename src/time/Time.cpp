#include "time/Time.h"

#include <iomanip>
#include <sstream>

std::string to_string_fixed(float64_t val, uint8_t n = 2) {
	std::ostringstream out;
	out << std::setprecision(n) << std::fixed << val;
	return out.str();
}

namespace Time {

	std::time_t timestamp() {
		return std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count();
	}

	void sleep(std::time_t duration) {
		std::this_thread::sleep_for(std::chrono::microseconds(duration));
	}

	std::time_t minutes(float64_t minutes) {
		return minutes * 1000000 * 60;
	}

	std::time_t seconds(float64_t seconds) {
		return seconds * 1000000;
	}

	std::time_t milliseconds(float64_t milliseconds) {
		return milliseconds * 1000;
	}

	std::string format(std::time_t microseconds) {
		auto milliseconds = microseconds / 1000.0;
		if (milliseconds < 1) {
			return to_string_fixed(microseconds) + "μs";
		}
		auto seconds = milliseconds / 1000.0;
		if (seconds < 1) {
			return to_string_fixed(milliseconds) + "ms";
		}
		auto minutes = seconds / 60.0;
		if (minutes < 1) {
			return to_string_fixed(seconds) + "s";
		}
		auto hours = minutes / 60.0;
		if (hours < 1) {
			return to_string_fixed(minutes) + "m";
		}
		return to_string_fixed(hours) + "h";
	}

}
