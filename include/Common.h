#pragma once

#include <enet/enet.h>

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

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

std::time_t timestamp();

void sleepMS(uint32_t);

std::string addressToString(const ENetAddress*);
