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
	std::time_t fromMinutes(float64_t);
	std::time_t fromSeconds(float64_t);
	std::time_t fromMilliseconds(float64_t);

	/**
	 * Converts the arguments from microseconds to the specified type.
	 */
	float64_t toMinutes(std::time_t);
	float64_t toSeconds(std::time_t);
	float64_t toMilliseconds(std::time_t);

	/**
	 * Format microsecond into a pretty string.
	 */
	std::string format(std::time_t);

}
