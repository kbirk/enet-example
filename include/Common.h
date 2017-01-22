#pragma once

#include <ctime>

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
 * Define EPSILON
 */
#ifndef M_EPSILON
	#define M_EPSILON 0.000001
#endif

/**
 * Utility lookup for const map types.
 */
template <typename T>
typename T::mapped_type get(T const& map, typename T::key_type const& key)
{
	typename T::const_iterator iter(map.find(key));
	return iter != map.end() ? iter->second : typename T::mapped_type();
}
