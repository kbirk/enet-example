#pragma once

#include "Common.h"
#include "input/Input.h"
#include "game/State.h"
#include "game/StateMachine.h"
#include "game/Player.h"
#include "math/Transform.h"
#include "serial/StreamBuffer.h"

class Camera {

	public:

		typedef std::shared_ptr<Camera> Shared;
		static Shared alloc(float32_t);
		static Shared alloc(float32_t, Transform::Shared);

		Camera(float32_t);
		Camera(float32_t, Transform::Shared);

		void update(std::time_t);
		void follow(Player::Shared);

		Transform::Shared transform();

		void zoom(float32_t delta);
		void rotate(glm::vec2 delta);

		glm::vec3 mouseToWorld(glm::vec2, float32_t, float32_t);

		void setPerspective(float32_t fov, float32_t aspect, float32_t near, float32_t far);
		void setAspect(float32_t);
		glm::mat4 projection() const;

	private:

		void updateRotation(std::time_t);
		void updateZoom(std::time_t);

		// prevent copy-construction
		Camera(const Camera&);
		// prevent assignment
		Camera& operator= (const Camera&);

		Transform::Shared transform_;
		Player::Shared target_;
		float32_t distance_;
		float32_t zoomVelocity_;
		glm::vec2 rotationVelocity_;
		float32_t fov_;
		float32_t aspect_;
		float32_t near_;
		float32_t far_;
		// StateMachine::Shared state_;
};
