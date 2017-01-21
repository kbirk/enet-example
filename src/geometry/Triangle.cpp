#include "geometry/Triangle.h"

Triangle::Shared Triangle::alloc() {
	return std::make_shared<Triangle>();
}

Triangle::Shared Triangle::alloc(const glm::vec3& n, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2) {
	return std::make_shared<Triangle>(n, p0, p1, p2);
}

Triangle::Shared Triangle::alloc(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2) {
	return std::make_shared<Triangle>(p0, p1, p2);
}

Triangle::Triangle()
	: points_(3, glm::vec3(0, 0, 0))
	, normal_(0, 0, 0)
	, centroid_(0, 0, 0)
	, radius_(0) {
}

Triangle::Triangle(const glm::vec3& n, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
	: normal_(n)
	, centroid_((1.0 / 3.0) * (p0 + p1 + p2)) {
	points_.push_back(p0);
	points_.push_back(p1);
	points_.push_back(p2);
	calcRadius();
}

Triangle::Triangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
	: normal_(glm::normalize(glm::cross(p1 - p0, p2 - p0)))
	, centroid_((1.0 / 3.0) * (p0 + p1 + p2)) {
	points_.push_back(p0);
	points_.push_back(p1);
	points_.push_back(p2);
	calcRadius();
}

const glm::vec3& Triangle::a() const {
	return points_[0];
}

const glm::vec3& Triangle::b() const {
	return points_[1];
}

const glm::vec3& Triangle::c() const {
	return points_[2];
}

const glm::vec3& Triangle::normal() const {
	return normal_;
}

const glm::vec3& Triangle::centroid() const {
	return centroid_;
}

float32_t Triangle::radius() const {
	return radius_;
}

void Triangle::recalculate() {
	normal_ = glm::normalize(glm::cross(points_[1] - points_[0], points_[2] - points_[0]));
	centroid_ = (1.0 / 3.0) * (points_[0] + points_[1] + points_[2]);
	calcRadius();
}

void Triangle::calcRadius() {
	auto a = glm::length(points_[0] - centroid_);
	auto b = glm::length(points_[1] - centroid_);
	auto c = glm::length(points_[2] - centroid_);
	radius_ = std::max(std::max(a, b), c);
}

Intersection Triangle::intersect(
	const glm::vec3& ray,
	const glm::vec3& origin,
	bool ignoreBehindRay,
	bool backFaceCull) const {

	// compute ray/plane intersection
	float32_t dn = glm::dot(ray, normal_);
	if (dn == 0 || (backFaceCull && dn > 0)) {
		// ray is parallel to plane, or coming from behind
		return Intersection();
	}

	float32_t t = glm::dot(points_[0] - origin, normal_) * (1 / dn);
	if (ignoreBehindRay && t < 0) {
		// plane is behind ray
		return Intersection();
	}

	glm::vec3 intersection = origin + t * ray;

	// check if point is inside the triangle
	if (!contains(intersection)) {
		return Intersection();
	}
	return Intersection(intersection, normal_, t);
}

bool Triangle::contains(const glm::vec3& point) const {
	// compute barycentric coords
	float32_t totalAreaDiv = 1.0 / glm::dot(glm::cross(points_[1]-points_[0], points_[2]-points_[0]), normal_);
	float32_t u = glm::dot(glm::cross(points_[2]-points_[1], point - points_[1]), normal_) * totalAreaDiv;
	float32_t v = glm::dot(glm::cross(points_[0]-points_[2], point - points_[2]), normal_) * totalAreaDiv;
	// reject if outside triangle
	if (u < 0 || v < 0 || u + v > 1) {
		return false;
	}
	return true;
}

glm::vec3 Triangle::closestPointTo(const glm::vec3& point) const {
	glm::vec3 e0 = closestPointOnEdge(0, point);
	glm::vec3 e1 = closestPointOnEdge(1, point);
	glm::vec3 e2 = closestPointOnEdge(2, point);
	float32_t d0 = glm::length2(e0 - point);
	float32_t d1 = glm::length2(e1 - point);
	float32_t d2 = glm::length2(e2 - point);
	if (d0 < d1) {
		if (d0 < d2) {
			return e0;
		} else {
			return e2;
		}
	} else {
		if (d1 < d2) {
			return e1;
		} else {
			return e2;
		}
	}
}

glm::vec3 Triangle::closestPointOnEdge(uint32_t edgeIndex, const glm::vec3& point) const {
	auto a = points_[edgeIndex];
	auto b = points_[(edgeIndex + 1) % 3];
	auto ab = b - a;
	// project c onto ab, computing parameterized position d(t) = a + t*(b * a)
	float32_t t = glm::dot(point - a, ab) / glm::dot(ab,  ab);
	// if outside segment, clamp t (and therefore d) to the closest endpoint
	if (t < 0.0f) {
		t = 0.0f;
	}
	if (t > 1.0f) {
		t = 1.0f;
	}
	// compute projected position from the clamped t
	return a + t * ab;
}

bool operator== (const Triangle::Shared& a, const Triangle::Shared& b) {
	return
		a->points_[0] == b->points_[0] &&
		a->points_[1] == b->points_[1] &&
		a->points_[2] == b->points_[2] &&
		a->normal_ == b->normal_;
}

bool operator!= (const Triangle::Shared& a, const Triangle::Shared& b) {
	return !(a == b);
}
