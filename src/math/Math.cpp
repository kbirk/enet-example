#include "math/Math.h"

namespace Math {

	glm::vec3 project(glm::vec3 a, glm::vec3 b) {
		return a - (b * glm::dot(a, b));
	}

	float32_t signedAngle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& normal) {
		auto cross = glm::cross(a, b);
		auto pa = glm::normalize(project(a, normal));
		auto pb = glm::normalize(project(b, normal));
		auto dot = glm::dot(pa, pb);
		auto angle = std::atan2(glm::length(glm::cross(pa, pb)), dot );
		if (std::isnan(angle)) {
			return 0;
		}
		if (glm::dot(normal, cross) < 0) {
			// or > 0
			angle = -angle;
		}
		return angle;
	}

}
