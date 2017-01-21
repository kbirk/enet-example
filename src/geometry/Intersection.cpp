#include "geometry/Intersection.h"

Intersection::Intersection()
	: hit(false)
	, position(0, 0, 0)
	, normal(0, 0, 1)
	, t(0) {
}

Intersection::Intersection(const glm::vec3& position, const glm::vec3& normal, float32_t& t)
	: hit(true)
	, position(position)
	, normal(normal)
	, t(t) {
}

StreamBuffer::Shared& operator<< (StreamBuffer::Shared& stream, const Intersection& intersection) {
	stream << intersection.hit;
	stream << intersection.position;
	stream << intersection.normal;
	stream << intersection.t;
	return stream;
}

StreamBuffer::Shared& operator>> (StreamBuffer::Shared& stream, Intersection& intersection) {
	stream >> intersection.hit;
	stream >> intersection.position;
	stream >> intersection.normal;
	stream >> intersection.t;
	return stream;
}
