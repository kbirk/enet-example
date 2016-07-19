#pragma once

#include "Common.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

class Geometry {

	public:

		typedef std::shared_ptr<Geometry> Shared;
		static Shared alloc();

		Geometry();

		void setPositions(const std::vector<glm::vec3>&);
		void setNormals(const std::vector<glm::vec3>&);
		void setUVs(const std::vector<glm::vec2>&);
		void setIndices(const std::vector<uint32_t>&);

		const std::vector<glm::vec3>& positions() const;
		const std::vector<glm::vec3>& normals() const;
		const std::vector<glm::vec2>& uvs() const;
		const std::vector<uint32_t>& indices() const;

	private:

		// prevent copy-construction
		Geometry(const Geometry&);
		// prevent assignment
		Geometry& operator= (const Geometry&);

		std::vector<glm::vec3> positions_;
		std::vector<glm::vec3> normals_;
		std::vector<glm::vec2> uvs_;
		std::vector<uint32_t> indices_;
};
