#pragma once

#include "Common.h"

#include <ctime>
#include <chrono>
#include <string>
#include <thread>

namespace Time {

	/**
	 * Get timestamp in microseconds.
	 */
	std::time_t timestamp();

	/**
	 * Sleep thread for N microseconds.
	 */
	void sleep(std::time_t);

	/**
	 * Converts the arguments to microseconds.
	 */
	std::time_t minutes(float64_t);
	std::time_t seconds(float64_t);
	std::time_t milliseconds(float64_t);

	/**
	 * Format microsecond into a pretty string.
	 */
	std::string format(std::time_t);

}
