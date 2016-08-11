#include "math/Transform.h"

#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

Transform::Shared Transform::alloc() {
	return std::make_shared<Transform>();
}

Transform::Transform()
	: translation_(0, 0, 0)
	, rotation_(1, 0, 0, 0)
	, scale_(1, 1, 1) {
}

void Transform::setRotation(float32_t angle, const glm::vec3& axis) {
	rotation_ = glm::angleAxis(angle, glm::normalize(axis));
}

void Transform::setRotation(const glm::quat& rotation) {
	rotation_ = rotation;
}

void Transform::setTranslation(const glm::vec3& t) {
	translation_ = t;
}

void Transform::setScale(const glm::vec3& s) {
	scale_ = s;
}

void Transform::setScale(float32_t s) {
	scale_ = glm::vec3(s, s, s);
}

void Transform::rotateLocal(float32_t angle, const glm::vec3& axis) {
	return rotateGlobal(angle, glm::mat3(rotationMatrix()) * axis);
}

void Transform::rotateGlobal(float32_t angle, const glm::vec3& axis) {
	rotation_ = glm::angleAxis(angle, axis) * rotation_;
}

void Transform::rotateTo(const glm::vec3& target) {
	auto diff = translation_ - target;

	auto start = glm::normalize(-z());
	auto dest = glm::normalize(diff);

	float32_t cosTheta = glm::dot(start, dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f) {
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
		if (glm::length2(rotationAxis) < 0.01) {
			// bad luck, they were parallel, try again!
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);
		}
		rotationAxis = glm::normalize(rotationAxis);
		rotation_ = glm::angleAxis(180.0f, rotationAxis);
	}

	rotationAxis = glm::cross(start, dest);

	float32_t s = sqrt((1.0 + cosTheta) * 2.0);
	float32_t invs = 1.0 / s;

	rotation_ = glm::quat(
		s * 0.5f,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);
}

void Transform::translateGlobal(const glm::vec3& t) {
	translation_ = translation_ + t;
}

void Transform::translateLocal(const glm::vec3& t) {
	translation_ = translation_ +
		x() * t.x +
		y() * t.y +
		z() * t.z;
}

glm::mat4 Transform::matrix() const {
	return translationMatrix() * rotationMatrix() * scaleMatrix();
}

glm::mat4 Transform::viewMatrix(bool ignoreTranslation) const {
	if (ignoreTranslation) {
		return glm::lookAt(glm::vec3(0, 0, 0), -z(), y());
	}
	return glm::lookAt(translation_, translation_ - z(), y());
}

glm::mat4 Transform::rotationMatrix() const {
	return mat4_cast(rotation_);
}

glm::mat4 Transform::translationMatrix() const {
	return glm::translate(glm::mat4(1.0f), translation_);
}

glm::mat4 Transform::scaleMatrix() const {
	return glm::scale(glm::mat4(1.0f), scale_);
}

const glm::quat& Transform::rotation() const {
	return rotation_;
}

const glm::vec3& Transform::translation() const {
	return translation_;
}

const glm::vec3& Transform::scale() const {
	return scale_;
}

glm::vec3 Transform::x() const {
	float32_t yy = rotation_.y*rotation_.y,
		zz = rotation_.z*rotation_.z,
		xy = rotation_.x*rotation_.y,
		xz = rotation_.x*rotation_.z,
		yw = rotation_.y*rotation_.w,
		zw = rotation_.z*rotation_.w;
	return normalize(glm::vec3(1 - 2*yy - 2*zz, 2*xy + 2*zw, 2*xz - 2*yw));
}

glm::vec3 Transform::y() const {
	float32_t xx = rotation_.x*rotation_.x,
		zz = rotation_.z*rotation_.z,
		xy = rotation_.x*rotation_.y,
		xw = rotation_.x*rotation_.w,
		yz = rotation_.y*rotation_.z,
		zw = rotation_.z*rotation_.w;
	return normalize(glm::vec3(2*xy - 2*zw, 1 - 2*xx - 2*zz, 2*yz + 2*xw));
}

glm::vec3 Transform::z() const {
	float32_t xx = rotation_.x*rotation_.x,
		yy = rotation_.y*rotation_.y,
		xz = rotation_.x*rotation_.z,
		xw = rotation_.x*rotation_.w,
		yz = rotation_.y*rotation_.z,
		yw = rotation_.y*rotation_.w;
	return normalize(glm::vec3(2*xz + 2*yw, 2*yz - 2*xw, 1 - 2*xx - 2*yy));
}

StreamBuffer& operator<< (StreamBuffer& stream, const Transform& transform) {
	stream << transform.translation_
		<< transform.rotation_
		<< transform.scale_;
	return stream;
}

StreamBuffer& operator>> (StreamBuffer& stream, Transform& transform) {
	stream >> transform.translation_
		>> transform.rotation_
		>> transform.scale_;
	return stream;
}
