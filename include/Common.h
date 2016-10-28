#pragma once

#include <chrono>
#include <thread>

/**
 * Precision explicit typedefs
 */
typedef float float32_t;
typedef double float64_t;

/**
 * Fixed width integer types
 */
#include <cstdint>

/**
 * Define PI
 */
#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

/**
 * Get timestamp in milliseconds.
 */
std::time_t timestamp();

/**
 * Sleep thread for N milliseconds.
 */
void sleepMS(uint32_t);
