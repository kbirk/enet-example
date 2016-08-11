#pragma once

#include "Common.h"
#include "serial/StreamBuffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <memory>
#include <string>
#include <map>

class Transform {

	public:

		typedef std::shared_ptr<Transform> Shared;
		static Shared alloc();

		Transform();

		void setRotation(float32_t, const glm::vec3&);
		void setRotation(const glm::quat&);
		void setTranslation(const glm::vec3&);
		void setScale(const glm::vec3&);
		void setScale(float32_t);

		void rotateLocal(float32_t, const glm::vec3&);
		void rotateGlobal(float32_t, const glm::vec3&);
		void rotateTo(const glm::vec3&);

		void translateLocal(const glm::vec3&);
		void translateGlobal(const glm::vec3&);

		glm::mat4 matrix() const;
		glm::mat4 viewMatrix(bool = false) const;
		glm::mat4 rotationMatrix() const;
		glm::mat4 translationMatrix() const;
		glm::mat4 scaleMatrix() const;

		const glm::quat& rotation() const;
		const glm::vec3& translation() const;
		const glm::vec3& scale() const;

		glm::vec3 x() const;
		glm::vec3 y() const;
		glm::vec3 z() const;

		friend StreamBuffer::Shared& operator<< (StreamBuffer::Shared&, const Transform::Shared&);
		friend StreamBuffer::Shared& operator>> (StreamBuffer::Shared&, Transform::Shared&);

	private:

		glm::vec3 translation_;
		glm::quat rotation_;
		glm::vec3 scale_;
};
