#include "geometry/Sphere.h"

namespace Sphere {

	Geometry::Shared geometry(uint32_t slices, uint32_t stacks, float32_t radius) {
		auto geom = Geometry::alloc();
		geom->setPositions(positions(slices, stacks, radius));
		geom->setNormals(normals(slices, stacks));
		geom->setUVs(uvs(slices, stacks));
		geom->setIndices(indices(slices, stacks));
		return geom;
	}

	std::vector<glm::vec3> positions(uint32_t slices, uint32_t stacks, float32_t radius) {
		float32_t stackAngle = M_PI / float32_t(stacks);
		float32_t sliceAngle = 2 * M_PI / float32_t(slices);
		auto positions = std::vector<glm::vec3>((stacks + 1) * (slices + 1));
		for (uint32_t i=0; i<=stacks; i++) {
			float32_t r0 = radius * sin(i * stackAngle);
			float32_t y0 = radius * cos(i * stackAngle);
			uint32_t offset = i * (slices + 1);
			for (uint32_t j=0; j<=slices; j++) {
				float32_t x0 = r0 * sin(j * sliceAngle);
				float32_t z0 = r0 * cos(j * sliceAngle);
				positions[offset + j] = glm::vec3(x0, y0, z0);
			}
		}
		return positions;
	}

	std::vector<glm::vec3> normals(uint32_t slices, uint32_t stacks) {
		float32_t stackAngle = M_PI / float32_t(stacks);
		float32_t sliceAngle = 2 * M_PI / float32_t(slices);
		auto normals = std::vector<glm::vec3>((stacks + 1) * (slices + 1));
		for (uint32_t i=0; i<=stacks; i++) {
			float32_t r0 = sin(i * stackAngle);
			float32_t y0 = cos(i * stackAngle);
			float32_t offset = i * (slices + 1);
			for (uint32_t j=0; j<=slices; j++) {
				float32_t x0 = r0 * sin(j * sliceAngle);
				float32_t z0 = r0 * cos(j * sliceAngle);
				normals[offset + j] = glm::vec3(x0, y0, z0);
			}
		}
		return normals;
	}

	std::vector<glm::vec2> uvs(uint32_t slices, uint32_t stacks) {
		auto uvs = std::vector<glm::vec2>((stacks + 1) * (slices + 1));
		for (uint32_t i=0; i<=stacks; i++) {
			float32_t offset = i * (slices + 1);
			for (uint32_t j=0; j<=slices; j++) {
				uvs[offset + j] = glm::vec2(j / slices, 1 - (i / stacks));
			}
		}
		return uvs;
	}

	std::vector<uint32_t> indices(uint32_t slices, uint32_t stacks) {
		auto indices = std::vector<uint32_t>((stacks + 1) * (slices + 1) * 6);
		uint32_t vertexIndex = 0;
		for (uint32_t i=0; i<stacks; i++) {
			float32_t offset = i * (slices + 1) * 6;
			for (uint32_t j=0; j<=slices; j++) {
				indices[offset + j*6] = vertexIndex + slices + 1;
				indices[offset + j*6 + 1] = vertexIndex;
				indices[offset + j*6 + 2] = vertexIndex + slices;
				indices[offset + j*6 + 3] = vertexIndex + slices + 1;
				indices[offset + j*6 + 4] = vertexIndex + 1;
				indices[offset + j*6 + 5] = vertexIndex;
				vertexIndex++;
			}
		}
		return indices;
	}
}
