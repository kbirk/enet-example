#include "geometry/Geometry.h"

Geometry::Shared Geometry::alloc() {
	return std::make_shared<Geometry>();
}

Geometry::Geometry() {
}

void Geometry::setPositions(const std::vector<glm::vec3>& positions) {
	positions_ = positions;
}

void Geometry::setNormals(const std::vector<glm::vec3>& normals) {
	normals_ = normals;
}

void Geometry::setUVs(const std::vector<glm::vec2>& uvs) {
	uvs_ = uvs;
}

void Geometry::setIndices(const std::vector<uint32_t>& indices) {
	indices_ = indices;
}

const std::vector<glm::vec3>& Geometry::positions() const {
	return positions_;
}

const std::vector<glm::vec3>& Geometry::normals() const {
	return normals_;
}

const std::vector<glm::vec2>& Geometry::uvs() const {
	return uvs_;
}

const std::vector<uint32_t>& Geometry::indices() const {
	return indices_;
}
