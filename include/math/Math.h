#pragma once

#include "Common.h"

#include <glm/glm.hpp>

namespace Math {

	float32_t signedAngle(
		const glm::vec3& a,
		const glm::vec3& b,
		const glm::vec3& normal);
}
