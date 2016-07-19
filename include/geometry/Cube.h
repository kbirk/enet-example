#pragma once

#include "Common.h"
#include "geometry/Geometry.h"

#include <glm/glm.hpp>

#include <vector>

const uint32_t CUBE_SIZE = 1;

namespace Cube {

	Geometry::Shared geometry(uint32_t size = CUBE_SIZE);
	std::vector<glm::vec3> positions(uint32_t size = CUBE_SIZE);
	std::vector<glm::vec3> normals();
	std::vector<glm::vec2> uvs();
	std::vector<uint32_t> indices();
}
