#include "game/Environment.h"

Environment::Shared Environment::alloc() {
	return std::make_shared<Environment>();
}

Environment::Environment() {
}

void Environment::addTerrain(uint32_t id, Terrain::Shared terrain) {
	terrain_[id] = terrain;
}

void Environment::removeTerrain(uint32_t id) {
	terrain_.erase(id);
}

const std::map<uint32_t, Terrain::Shared>& Environment::terrain() const {
	return terrain_;
}

Intersection Environment::intersect(const glm::vec3& ray, const glm::vec3& origin, bool ignoreBehindRay, bool backFaceCull) const {
	Intersection closest;
	float32_t min = std::numeric_limits<float32_t>::max();
	for (auto iter : terrain_) {
		auto terrain = iter.second;
		auto intersection = terrain->intersect(ray, origin, ignoreBehindRay, backFaceCull);
		if (intersection.hit) {
			auto dist = glm::length2(origin - intersection.position);
			if (dist < min) {
				closest = intersection;
				min = dist;
			}
		}
	}
	return closest;
}
