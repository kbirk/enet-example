#include "serial/Serialization.h"

#include <cstring>

uint64_t pack754(float64_t f, uint32_t bits, uint32_t expbits)
{
    float64_t fnorm;
    int32_t shift;
    int64_t sign, expo, significand;
    uint32_t significandbits = bits - expbits - 1; // -1 for sign bit
    if (f == 0.0) {
        // get this special case out of the way
        return 0;
    }
    // check sign and begin normalization
    if (f < 0) {
        sign = 1;
        fnorm = -f;
    } else {
        sign = 0;
        fnorm = f;
    }
    // get the normalized form of f and track the exponent
    shift = 0;
    while (fnorm >= 2.0) {
        fnorm /= 2.0;
        shift++;
    }
    while (fnorm < 1.0) {
        fnorm *= 2.0;
        shift--;
    }
    fnorm = fnorm - 1.0;
    // calculate the binary form (non-float) of the significand data
    significand = fnorm * ((1LL << significandbits) + 0.5f);
    // get the biased exponent
    expo = shift + ((1 << (expbits - 1)) - 1); // shift + bias
    // return the final answer
    return (sign << (bits - 1)) | (expo << (bits - expbits - 1)) | significand;
}

float64_t unpack754(uint64_t i, uint32_t bits, uint32_t expbits)
{
    float64_t result;
    int64_t shift;
    uint32_t bias;
    uint32_t significandbits = bits - expbits - 1; // -1 for sign bit
    if (i == 0) {
        // get this special case out of the way
        return 0.0;
    }
    // pull the significand
    result = (i & ((1LL << significandbits) - 1)); // mask
    result /= (1LL << significandbits); // convert back to float
    result += 1.0f; // add the one back on
    // deal with the exponent
    bias = (1 << (expbits - 1)) - 1;
    shift = ((i >> significandbits) & ((1LL << expbits) - 1)) - bias;
    while (shift > 0) {
        result *= 2.0;
        shift--;
    }
    while (shift < 0) {
        result /= 2.0;
        shift++;
    }
    // sign it
    result *= (i >> (bits - 1)) & 1 ? -1.0 : 1.0;
    return result;
}
