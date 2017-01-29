#include "geometry/Triangle.h"

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>

Triangle::Shared Triangle::alloc() {
	return std::make_shared<Triangle>();
}

Triangle::Shared Triangle::alloc(const glm::vec3& n, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
	return std::make_shared<Triangle>(n, a, b, c);
}

Triangle::Shared Triangle::alloc(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
	return std::make_shared<Triangle>(a, b, c);
}

Triangle::Triangle()
	: a_(0, 0, 0)
	, b_(0, 0, 0)
	, c_(0, 0, 0)
	, normal_(0, 0, 0)
	, centroid_(0, 0, 0)
	, radius_(0) {
}

Triangle::Triangle(const glm::vec3& n, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
	: a_(a)
	, b_(b)
	, c_(c)
	, normal_(n)
	, centroid_((1.0 / 3.0) * (a + b + c)) {
	calcRadius();
}

Triangle::Triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
	: a_(a)
	, b_(b)
	, c_(c)
	, normal_(glm::normalize(glm::cross(b - a, c - a)))
	, centroid_((1.0 / 3.0) * (a + b + c)) {
	calcRadius();
}

const glm::vec3& Triangle::a() const {
	return a_;
}

const glm::vec3& Triangle::b() const {
	return b_;
}

const glm::vec3& Triangle::c() const {
	return c_;
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
	normal_ = glm::normalize(glm::cross(b_ - a_, c_ - a_));
	centroid_ = (1.0 / 3.0) * (a_ + b_ + c_);
	calcRadius();
}

void Triangle::calcRadius() {
	auto a = glm::length(a_ - centroid_);
	auto b = glm::length(b_ - centroid_);
	auto c = glm::length(c_ - centroid_);
	radius_ = std::max(std::max(a, b), c);
}

Intersection Triangle::intersect(
	const glm::vec3& ray,
	const glm::vec3& origin,
	bool ignoreBehindRay,
	bool backFaceCull) const {

	glm::vec3 bary;
	//
	// if (glm::intersectLineTriangle(origin, ray, a_, b_, c_, bary)) {
	// 	auto pos = origin + ray * bary.z;
	// 	return Intersection(pos, normal_, bary.z);
	// }

	if (glm::intersectRayTriangle(origin, ray, a_, b_, c_, bary)) {
		auto pos = origin + ray * bary.z;
		return Intersection(pos, normal_, bary.z);
	}
	if (glm::intersectRayTriangle(origin, -ray, a_, c_, b_, bary)) {
		auto pos = origin + -ray * bary.z;
		return Intersection(pos, normal_, bary.z);
	}
	return Intersection();

	/*
	// compute ray/plane intersection
	float32_t dn = glm::dot(ray, normal_);
	if (dn == 0 || (backFaceCull && dn > 0)) {
		// ray is parallel to plane, or coming from behind
		return Intersection();
	}

	float32_t t = glm::dot(a_ - origin, normal_) / dn;
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
	*/
}

bool Triangle::contains(const glm::vec3& point) const {
	// compute barycentric coords
	float32_t totalAreaDiv = 1.0 / glm::dot(glm::cross(b_ - a_, c_ - a_), normal_);
	float32_t u = glm::dot(glm::cross(c_ - b_, point - b_), normal_) * totalAreaDiv;
	float32_t v = glm::dot(glm::cross(a_ - c_, point - c_), normal_) * totalAreaDiv;
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
	glm::vec3 a, b;
	switch (edgeIndex) {
		case 0:
			a = a_;
			b = b_;
			break;
		case 1:
			a = b_;
			b = c_;
			break;
		default:
			a = c_;
			b = a_;
			break;
	}
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
		a->a_ == b->a_ &&
		a->b_ == b->b_ &&
		a->c_ == b->c_ &&
		a->normal_ == b->normal_;
}

bool operator!= (const Triangle::Shared& a, const Triangle::Shared& b) {
	return !(a == b);
}
