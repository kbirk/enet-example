#include "serial/Serialization.h"

#include <cstring>

uint64_t pack754(float64_t f, uint32_t bits, uint32_t expbits) {
	float64_t fnorm;
	int shift;
	int64_t sign, expo, significand;
	uint32_t significandbits = bits - expbits - 1; // -1 for sign bit
	if (f == 0.0) {
		// get this special case out of the way
		return 0;
	}
	// check sign and begin normalization
	if (f < 0) {
		sign = 1; fnorm = -f;
	} else {
		sign = 0; fnorm = f;
	}
	// get the normalized form of f and track the exponent
	shift = 0;
	while(fnorm >= 2.0) {
		fnorm /= 2.0;
		shift++;
	}
	while(fnorm < 1.0) {
		fnorm *= 2.0;
		shift--;
	}
	fnorm = fnorm - 1.0;
	// calculate the binary form (non-float) of the significand data
	significand = fnorm * ((1LL<<significandbits) + 0.5f);
	// get the biased exponent
	expo = shift + ((1<<(expbits-1)) - 1); // shift + bias
	// return the final answer
	return (sign<<(bits-1)) | (expo<<(bits-expbits-1)) | significand;
}

float64_t unpack754(uint64_t i, uint32_t bits, uint32_t expbits) {
	float64_t result;
	int64_t shift;
	uint32_t bias;
	uint32_t significandbits = bits - expbits - 1; // -1 for sign bit
	if (i == 0) {
		// get this special case out of the way
		return 0.0;
	}
	// pull the significand
	result = (i&((1LL<<significandbits)-1)); // mask
	result /= (1LL<<significandbits); // convert back to float
	result += 1.0f; // add the one back on
	// deal with the exponent
	bias = (1<<(expbits-1)) - 1;
	shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
	while(shift > 0) { result *= 2.0; shift--; }
	while(shift < 0) { result /= 2.0; shift++; }
	// sign it
	result *= (i>>(bits-1))&1? -1.0: 1.0;
	return result;
}

uint32_t serialize(uint8_t* dest, uint32_t x, uint32_t offset) {
	memcpy(dest + offset, &x, 4);
	return 4;
}

uint32_t deserialize(uint32_t* x, const uint8_t* src, uint32_t offset) {
	memcpy(x, src + offset, 4);
	return 4;
}

uint32_t serialize(uint8_t* dest, const glm::vec3& vec, uint32_t offset) {
	if (dest == nullptr) {
		return 12;
	}
	uint32_t x = pack754_32(vec.x);
	uint32_t y = pack754_32(vec.y);
	uint32_t z = pack754_32(vec.z);
	memcpy(dest + offset, &x, 4);
	memcpy(dest + offset + 4, &y, 4);
	memcpy(dest + offset + 8, &z, 4);
	return 12;
}

uint32_t serialize(uint8_t* dest, const glm::quat& quat, uint32_t offset) {
	if (dest == nullptr) {
		return 16;
	}
	uint32_t x = pack754_32(quat.x);
	uint32_t y = pack754_32(quat.y);
	uint32_t z = pack754_32(quat.z);
	uint32_t w = pack754_32(quat.w);
	memcpy(dest + offset, &x, 4);
	memcpy(dest + offset + 4, &y, 4);
	memcpy(dest + offset + 8, &z, 4);
	memcpy(dest + offset + 12, &w, 4);
	return 16;
}

uint32_t deserialize(glm::vec3& vec, const uint8_t* src, uint32_t offset) {
	if (src == nullptr) {
		return 12;
	}
	vec.x = unpack754_32(*((uint32_t*)(src + offset)));
	vec.y = unpack754_32(*((uint32_t*)(src + offset + 4)));
	vec.z = unpack754_32(*((uint32_t*)(src + offset + 8)));
	return 12;
}

uint32_t deserialize(glm::quat& quat, const uint8_t* src, uint32_t offset) {
	if (src == nullptr) {
		return 16;
	}
	quat.x = unpack754_32(*((uint32_t*)(src + offset)));
	quat.y = unpack754_32(*((uint32_t*)(src + offset + 4)));
	quat.z = unpack754_32(*((uint32_t*)(src + offset + 8)));
	quat.w = unpack754_32(*((uint32_t*)(src + offset + 12)));
	return 16;
}
