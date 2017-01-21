#include "geometry/Octree.h"

#include "gl/ElementArrayBufferObject.h"
#include "gl/VertexAttributePointer.h"

#include <limits>

Octree::Shared Octree::alloc() {
	return std::make_shared<Octree>();
}

Octree::Shared Octree::alloc(const std::vector<Triangle::Shared>& triangles, uint32_t depth) {
	return std::make_shared<Octree>(triangles, depth);
}

Octree::Shared Octree::alloc(const glm::vec3& center, float32_t halfWidth, uint32_t depth) {
	return std::make_shared<Octree>(center, halfWidth, depth);
}

Octree::Octree()
	: children_(8, nullptr) {
}

Octree::Octree(const std::vector<Triangle::Shared>& triangles, uint32_t depth)
	: children_(8, nullptr) {

	glm::vec3 min(
		std::numeric_limits<float32_t>::max(),
		std::numeric_limits<float32_t>::max(),
		std::numeric_limits<float32_t>::max());
	glm::vec3 max(
		std::numeric_limits<float32_t>::min(),
		std::numeric_limits<float32_t>::min(),
		std::numeric_limits<float32_t>::min());
	for (auto tri : triangles) {
		// min
		min.x = std::min(min.x, tri->a().x);
		min.x = std::min(min.x, tri->b().x);
		min.x = std::min(min.x, tri->c().x);
		min.y = std::min(min.y, tri->a().y);
		min.y = std::min(min.y, tri->b().y);
		min.y = std::min(min.y, tri->c().y);
		min.z = std::min(min.z, tri->a().z);
		min.z = std::min(min.z, tri->b().z);
		min.z = std::min(min.z, tri->c().z);
		// max
		max.x = std::max(max.x, tri->a().x);
		max.x = std::max(max.x, tri->b().x);
		max.x = std::max(max.x, tri->c().x);
		max.y = std::max(max.y, tri->a().y);
		max.y = std::max(max.y, tri->b().y);
		max.y = std::max(max.y, tri->c().y);
		max.z = std::max(max.z, tri->a().z);
		max.z = std::max(max.z, tri->b().z);
		max.z = std::max(max.z, tri->c().z);
	}

	// build from root
	build(min, max, triangles, depth);
}

Octree::Octree(
	const glm::vec3& center,
	float32_t halfWidth,
	uint32_t depth)
	: depth_(depth)
	, center_(center)
	, halfWidth_(halfWidth)
	, children_(8, nullptr) {
}

Octree::~Octree () {
	clear();
}

void Octree::clear() {
	for (auto child : children_) {
		if (child) {
			child->clear();
		}
	}
	children_ = std::vector<Octree::Shared>(8, nullptr);
}

void Octree::build(const glm::vec3& min, const glm::vec3& max, const std::vector<Triangle::Shared>& triangles, uint32_t depth) {
	// center point of octree
	center_ = 0.5f * (min + max);
	depth_ = depth;
	// find largest distance component to become half width
	glm::vec3 minDiff = min - center_;
	glm::vec3 maxDiff = max - center_;
	float32_t minMax = std::max(std::max(fabs(minDiff.x), fabs(minDiff.y)), fabs(minDiff.z));
	float32_t maxMax = std::max(std::max(fabs(maxDiff.x), fabs(maxDiff.y)), fabs(maxDiff.z));
	// half width of octree cell
	halfWidth_ = std::max(minMax, maxMax);
	// clear just incase
	clear();
	// insert triangles into octree
	for (auto triangle : triangles) {
		insert(triangle);
	}
}

void Octree::insert(Triangle::Shared tri) {
	// only add triangle if leaf node
	if (depth_ == 0) {
		contained_.push_back(tri);
	}

	// distance from each axis
	float32_t dx = tri->centroid().x - center_.x;
	float32_t dy = tri->centroid().y - center_.y;
	float32_t dz = tri->centroid().z - center_.z;

	// if distance is less than radius, then the triangle straddles a boundary
	if (fabs(dx) < tri->radius() ||
		fabs(dy) < tri->radius() ||
		fabs(dz) < tri->radius()) {
		// straddles a boundary try to add to intersected children
		for (uint32_t i=0; i<8; i++) {
			// check if triangle bounding sphere intersects this child
			if (sphereCheck(tri->centroid(), tri->radius(), i)) {
				// part of bounding sphere intersects child, insert
				insertIntoChild(i, tri);
			}
		}
	} else {
		// fully contained in a single child, find child index
		// contains the 0-7 index of the child, determined using bit wise addition
		int32_t child = 0;
		if (dx > 0) child += 1;
		if (dy > 0) child += 2;
		if (dz > 0) child += 4;
		insertIntoChild(child, tri);
	}
}

void Octree::insertIntoChild(uint32_t index, Triangle::Shared tri) {
	if (children_[index]) {
		// child already exists, recursively insert
		children_[index]->insert(tri);
	} else {
		// child does not exist, if terminal depth has not been reached, create child node
		if (depth_ > 0) {
			float32_t step = halfWidth_ * 0.5f;
			glm::vec3 offset;
			offset.x = ((index & 1) ? step : -step);
			offset.y = ((index & 2) ? step : -step);
			offset.z = ((index & 4) ? step : -step);
			children_[index] = Octree::alloc(center_ + offset, step, depth_-1);
			children_[index]->insert(tri);
		}
	}
}

VertexArrayObject::Shared Octree::generateVAO(float32_t halfWidth) const {
	std::vector<glm::vec3> positions;
	positions.reserve(8);

	// front face
	positions.emplace_back(glm::vec3(-halfWidth, -halfWidth, halfWidth));
	positions.emplace_back(glm::vec3(halfWidth, -halfWidth, halfWidth));
	positions.emplace_back(glm::vec3(halfWidth, halfWidth, halfWidth));
	positions.emplace_back(glm::vec3(-halfWidth, halfWidth, halfWidth));

	// back face
	positions.emplace_back(glm::vec3(-halfWidth, -halfWidth, -halfWidth));
	positions.emplace_back(glm::vec3(halfWidth, -halfWidth, -halfWidth));
	positions.emplace_back(glm::vec3(halfWidth,  halfWidth, -halfWidth));
	positions.emplace_back(glm::vec3(-halfWidth,  halfWidth, -halfWidth));

	std::vector<uint32_t> indices;
	indices.reserve(8*3);

	// front
	indices.push_back(0);   indices.push_back(1);
	indices.push_back(1);   indices.push_back(2);
	indices.push_back(2);   indices.push_back(3);
	indices.push_back(3);   indices.push_back(0);

	// sides
	indices.push_back(0);   indices.push_back(4);
	indices.push_back(1);   indices.push_back(5);
	indices.push_back(2);   indices.push_back(6);
	indices.push_back(3);   indices.push_back(7);

	// back
	indices.push_back(4);   indices.push_back(5);
	indices.push_back(5);   indices.push_back(6);
	indices.push_back(6);   indices.push_back(7);
	indices.push_back(7);   indices.push_back(4);

	// positions
	auto vbo = VertexBufferObject::alloc();
	vbo->upload(positions);
	// indices
	auto eabo = ElementArrayBufferObject::alloc(GL_UNSIGNED_INT, GL_LINES);
	eabo->upload(indices);
	// create and attach to vao
	auto vao = VertexArrayObject::alloc();
	vao->attach(vbo, VertexAttributePointer::alloc(0, 3, GL_FLOAT));
	vao->attach(eabo);
	// upload to gpu
	vao->upload();
	return vao;
}

float32_t Octree::sqrDistFromPoint(const glm::vec3& point, int32_t child) const {
	// shift AABB dimesions based on which child cell is begin tested
	glm::vec3 offsetCenter = center_;
	float32_t step = 0.5f * halfWidth_;
	offsetCenter.x += ((child & 1) ? step : -step);
	offsetCenter.y += ((child & 2) ? step : -step);
	offsetCenter.z += ((child & 4) ? step : -step);
	glm::vec3 minAABB = glm::vec3(offsetCenter.x - step, offsetCenter.y - step, offsetCenter.z - step);
	glm::vec3 maxAABB = glm::vec3(offsetCenter.x + step, offsetCenter.y + step, offsetCenter.z + step);

	// For each axis count any excess distance outside box extents
	float32_t sqrDist = 0.0f;
	// x
	if (point.x < minAABB.x) sqrDist += (minAABB.x - point.x) * (minAABB.x - point.x);
	if (point.x > maxAABB.x) sqrDist += (point.x - maxAABB.x) * (point.x - maxAABB.x);
	// y
	if (point.y < minAABB.y) sqrDist += (minAABB.y - point.y) * (minAABB.y - point.y);
	if (point.y > maxAABB.y) sqrDist += (point.y - maxAABB.y) * (point.y - maxAABB.y);
	// z
	if (point.z < minAABB.z) sqrDist += (minAABB.z - point.z) * (minAABB.z - point.z);
	if (point.z > maxAABB.z) sqrDist += (point.z - maxAABB.z) * (point.z - maxAABB.z);

	return sqrDist;
}

glm::vec3 Octree::closestPointInBox(const glm::vec3& point, int32_t child) const {
	// shift AABB dimesions based on which child cell is begin tested
	float32_t step = 0.5f * halfWidth_;
	glm::vec3 offsetCenter = center_;
	offsetCenter.x += ((child & 1) ? step : -step);
	offsetCenter.y += ((child & 2) ? step : -step);
	offsetCenter.z += ((child & 4) ? step : -step);
	glm::vec3 minAABB = glm::vec3(offsetCenter.x - step, offsetCenter.y - step, offsetCenter.z - step);
	glm::vec3 maxAABB = glm::vec3(offsetCenter.x + step, offsetCenter.y + step, offsetCenter.z + step);

	glm::vec3 closestPoint;
	closestPoint.x = (point.x < minAABB.x)? minAABB.x : (point.x > maxAABB.x)? maxAABB.x : point.x;
	closestPoint.y = (point.y < minAABB.y)? minAABB.y : (point.y > maxAABB.y)? maxAABB.y : point.y;
	closestPoint.z = (point.z < minAABB.z)? minAABB.z : (point.z > maxAABB.z)? maxAABB.z : point.z;

	return closestPoint;
}

bool Octree::sphereCheck(const glm::vec3& center, float32_t radius, int32_t child) const {
	// compute squared distance between sphere center and AABB
	float32_t dist = sqrDistFromPoint(center, child);
	// sphere and AABB intersect if the distance is less than the sphere radius
	return dist <= radius*radius;
}

Intersection Octree::intersect(const glm::vec3& ray, const glm::vec3& origin, bool ignoreBehindRay, bool backFaceCull) const {
	// check if ray origin is inside box
	glm::vec3 diff = origin - center_;

	if (ignoreBehindRay) {
		if (fabs(diff.x) > halfWidth_ && diff.x*ray.x >= 0.0f) {
			return Intersection();
		}
		if (fabs(diff.y) > halfWidth_ && diff.y*ray.y >= 0.0f) {
			return Intersection();
		}
		if (fabs(diff.z) > halfWidth_ && diff.z*ray.z >= 0.0f) {
			return Intersection();
		}
	}

	glm::vec3 absDirection(fabs(ray.x), fabs(ray.y), fabs(ray.z));

	float32_t f = ray.y * diff.z - ray.z * diff.y;
	if (fabs(f) > halfWidth_*absDirection.z + halfWidth_*absDirection.y) {
		return Intersection();
	}

	f = ray.z * diff.x - ray.x * diff.z;
	if (fabs(f) > halfWidth_*absDirection.z + halfWidth_*absDirection.x) {
		return Intersection();
	}

	f = ray.x * diff.y - ray.y * diff.x;
	if (fabs(f) > halfWidth_*absDirection.y + halfWidth_*absDirection.x) {
		return Intersection();
	}

	// intersection exists, recurse further
	uint32_t childCount = 0;

	// find the closest intersection
	Intersection closest;
	float32_t min = std::numeric_limits<float32_t>::max();

	for (uint32_t i=0; i<8; i++) {
		if (children_[i]) {
			auto intersection = children_[i]->intersect(ray, origin, ignoreBehindRay, backFaceCull);
			// not a leaf, recurse
			if (intersection.hit) {
				auto dist = glm::length2(origin - intersection.position);
				if (dist < min) {
					closest = intersection;
					min = min;
				}
			}
			childCount++;
		}
	}

	// if leaf, intersect triangles
	if (childCount == 0) {
		for (auto tri : contained_) {
			auto intersection = tri->intersect(ray, origin, ignoreBehindRay, backFaceCull);
			if (intersection.hit) {
				auto dist = glm::length2(origin - intersection.position);
				if (dist < min) {
					closest = intersection;
					min = min;
				}
			}
		}
	}

	return closest;
}
