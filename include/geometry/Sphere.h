#pragma once

#include "Common.h"
#include "geometry/Geometry.h"

#include <glm/glm.hpp>

#include <vector>

const uint32_t RADIUS = 1;
const uint32_t STACKS = 20;
const uint32_t SLICES = 20;

namespace Sphere {

Geometry::Shared geometry(
    uint32_t slices = SLICES,
    uint32_t stacks = STACKS,
    float32_t radius = RADIUS);

std::vector<glm::vec3> positions(
    uint32_t slices = SLICES,
    uint32_t stacks = STACKS,
    float32_t radius = RADIUS);

std::vector<glm::vec3> normals(
    uint32_t slices = SLICES,
    uint32_t stacks = STACKS);

std::vector<glm::vec2> uvs(
    uint32_t slices = SLICES,
    uint32_t stacks = STACKS);

std::vector<uint32_t> indices(
    uint32_t slices = SLICES,
    uint32_t stacks = STACKS);
}
