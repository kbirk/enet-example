#include "geometry/Cube.h"

namespace Cube {

	Geometry::Shared geometry(uint32_t size) {
		auto geom = Geometry::alloc();
		geom->setPositions(positions(size));
		geom->setNormals(normals());
		geom->setUVs(uvs());
		geom->setIndices(indices());
		return geom;
	}

	std::vector<glm::vec3> positions(uint32_t size) {
		float32_t half = size / 2.0f;
		return std::vector<glm::vec3>{
			// front face
			glm::vec3(-half, -half, half),
			glm::vec3(half, -half, half),
			glm::vec3(half, half, half),
			glm::vec3(-half, half, half),
			// back face
			glm::vec3(-half, -half, -half),
			glm::vec3(-half, half, -half),
			glm::vec3(half, half, -half),
			glm::vec3(half, -half, -half),
			// top face
			glm::vec3(-half, half, -half),
			glm::vec3(-half, half, half),
			glm::vec3(half, half, half),
			glm::vec3(half, half, -half),
			// bottom face
			glm::vec3(-half, -half, -half),
			glm::vec3(half, -half, -half),
			glm::vec3(half, -half, half),
			glm::vec3(-half, -half, half),
			// right face
			glm::vec3(half, -half, -half),
			glm::vec3(half, half, -half),
			glm::vec3(half, half, half),
			glm::vec3(half, -half, half),
			// left face
			glm::vec3(-half, -half, -half),
			glm::vec3(-half, -half, half),
			glm::vec3(-half, half, half),
			glm::vec3(-half, half, -half)
		};
	}

	std::vector<glm::vec3> normals() {
		return std::vector<glm::vec3>{
			// front face
			glm::vec3(0.0, 0.0, 1.0),
			glm::vec3(0.0, 0.0, 1.0),
			glm::vec3(0.0, 0.0, 1.0),
			glm::vec3(0.0, 0.0, 1.0),
			// back face
			glm::vec3(0.0, 0.0, -1.0),
			glm::vec3(0.0, 0.0, -1.0),
			glm::vec3(0.0, 0.0, -1.0),
			glm::vec3(0.0, 0.0, -1.0),
			// top face
			glm::vec3(0.0, 1.0, 0.0),
			glm::vec3(0.0, 1.0, 0.0),
			glm::vec3(0.0, 1.0, 0.0),
			glm::vec3(0.0, 1.0, 0.0),
			// bottom face
			glm::vec3(0.0, -1.0, 0.0),
			glm::vec3(0.0, -1.0, 0.0),
			glm::vec3(0.0, -1.0, 0.0),
			glm::vec3(0.0, -1.0, 0.0),
			// right face
			glm::vec3(1.0, 0.0, 0.0),
			glm::vec3(1.0, 0.0, 0.0),
			glm::vec3(1.0, 0.0, 0.0),
			glm::vec3(1.0, 0.0, 0.0),
			// left face
			glm::vec3(-1.0, 0.0, 0.0),
			glm::vec3(-1.0, 0.0, 0.0),
			glm::vec3(-1.0, 0.0, 0.0),
			glm::vec3(-1.0, 0.0, 0.0)
		};
	}

	std::vector<glm::vec2> uvs() {
		return std::vector<glm::vec2>{
			// front face
			glm::vec2(0.0, 0.0),
			glm::vec2(1.0, 0.0),
			glm::vec2(1.0, 1.0),
			glm::vec2(0.0, 1.0),
			// back face
			glm::vec2(1.0, 0.0),
			glm::vec2(1.0, 1.0),
			glm::vec2(0.0, 1.0),
			glm::vec2(0.0, 0.0),
			// top face
			glm::vec2(0.0, 1.0),
			glm::vec2(0.0, 0.0),
			glm::vec2(1.0, 0.0),
			glm::vec2(1.0, 1.0),
			// bottom face
			glm::vec2(1.0, 1.0),
			glm::vec2(0.0, 1.0),
			glm::vec2(0.0, 0.0),
			glm::vec2(1.0, 0.0),
			// right face
			glm::vec2(1.0, 0.0),
			glm::vec2(1.0, 1.0),
			glm::vec2(0.0, 1.0),
			glm::vec2(0.0, 0.0),
			// left face
			glm::vec2(0.0, 0.0),
			glm::vec2(1.0, 0.0),
			glm::vec2(1.0, 1.0),
			glm::vec2(0.0, 1.0)
		};
	}

	std::vector<uint32_t> indices() {
		return std::vector<uint32_t>{
			0, 1, 2, 0, 2, 3, // front face
			4, 5, 6, 4, 6, 7, // back face
			8, 9, 10, 8, 10, 11, // top face
			12, 13, 14, 12, 14, 15, // bottom face
			16, 17, 18, 16, 18, 19, // right face
			20, 21, 22, 20, 22, 23 // left face
		};
	}
}
