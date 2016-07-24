#pragma once

#include "Common.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

/**
 * Bit packing macros
 * Brian "Beej Jorgensen" Hall
 * http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#serialization
 */
#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))
uint64_t pack754(float64_t f, uint32_t bits, uint32_t expbits);
float64_t unpack754(uint64_t i, uint32_t bits, uint32_t expbits);

/**
 * Unsigned integers
 */
uint32_t serialize(uint8_t*, uint32_t, uint32_t = 0);
uint32_t deserialize(uint32_t*, const uint8_t*, uint32_t = 0);

/**
 * GLM
 */
uint32_t serialize(uint8_t*, const glm::vec3&, uint32_t = 0);
uint32_t serialize(uint8_t*, const glm::quat&, uint32_t = 0);

uint32_t deserialize(glm::vec3&, const uint8_t*, uint32_t = 0);
uint32_t deserialize(glm::quat&, const uint8_t*, uint32_t = 0);
