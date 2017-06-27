#include "game/Camera.h"

#include "time/Time.h"

const float32_t SCROLL_DEFAULT_DISTANCE = 10.0f;
const float32_t SCROLL_FACTOR = 5.0f;
const float32_t SCROLL_MAX_DISTANCE = SCROLL_DEFAULT_DISTANCE * 10.0;
const float32_t SCROLL_MIN_DISTANCE = 0.0;
const float32_t ROTATE_X_FACTOR = -1.0;
const float32_t ROTATE_Y_FACTOR = -2.0;
const float32_t ZOOM_EASING_FACTOR = 10.0;
const float32_t ROTATE_EASING_FACTOR = 10.0;
const float32_t FOLLOW_EASING_FACTOR = 2.5;
const float32_t FIELD_OF_VIEW = 60.0f / (180.0f / M_PI);
const float32_t NEAR_PLANE = 0.1f;
const float32_t FAR_PLANE = 10000.0f;

Camera::Shared Camera::alloc(float32_t aspect)
{
    return std::make_shared<Camera>(aspect);
}

Camera::Shared Camera::alloc(float32_t aspect, Transform::Shared transform)
{
    return std::make_shared<Camera>(aspect, transform);
}

Camera::Camera(float32_t aspect)
    : Camera(aspect, Transform::alloc())
{
}

Camera::Camera(float32_t aspect, Transform::Shared transform)
    : transform_(transform)
    , distance_(SCROLL_DEFAULT_DISTANCE)
    , zoomVelocity_(0)
    , rotationVelocity_(0)
    , fov_(FIELD_OF_VIEW)
    , aspect_(aspect)
    , near_(NEAR_PLANE)
    , far_(FAR_PLANE)
{
}

void Camera::update(std::time_t dt)
{

    if (target_) {
        // update target position before resetting camera translation
        updateTarget(dt);
    }

    // set camera at origin
    transform_->setTranslation(glm::vec3(0, 0, 0));

    // apply rotation velocity
    updateRotation(dt);

    // apply zoom velocity
    updateZoom(dt);

    // where to center camera
    if (target_) {
        // appy target following
        transform_->setTranslation(targetPosition_);
    } else {
        transform_->setTranslation(glm::vec3(0, 0, 0));
    }

    // translate camera
    transform_->translateLocal(glm::vec3(0, 0, distance_));
}

void Camera::updateRotation(std::time_t dt)
{
    if (glm::length2(rotationVelocity_) < M_EPSILON) {
        return;
    }
    // update velocity
    auto fdt = Time::toSeconds(dt);
    auto delta = rotationVelocity_ * fdt;
    // rotate around origin
    transform_->rotateGlobal(delta.x, glm::vec3(0, 1, 0));
    transform_->rotateLocal(delta.y, glm::vec3(1, 0, 0));
    // decrement velocity
    auto friction = delta * ROTATE_EASING_FACTOR;
    rotationVelocity_ -= friction;
}

void Camera::updateZoom(std::time_t dt)
{
    if (fabs(zoomVelocity_) < M_EPSILON) {
        return;
    }
    // update distance
    auto fdt = Time::toSeconds(dt);
    auto delta = zoomVelocity_ * fdt;
    distance_ += delta;
    distance_ = std::min(std::max(distance_, SCROLL_MIN_DISTANCE), SCROLL_MAX_DISTANCE);
    // decrement velocity
    auto friction = delta * ZOOM_EASING_FACTOR;
    if ((zoomVelocity_ > 0 && zoomVelocity_ < friction) || (zoomVelocity_ < 0 && zoomVelocity_ > friction)) {
        zoomVelocity_ = 0;
    } else {
        zoomVelocity_ -= friction;
    }
}

void Camera::updateTarget(std::time_t dt)
{
    auto diff = target_->transform()->translation() - targetPosition_;
    auto fdt = Time::toSeconds(dt) * FOLLOW_EASING_FACTOR;
    auto dist = std::min(glm::length(diff), float32_t(fdt));
    if (dist < M_EPSILON) {
        return;
    }
    //auto direction = glm::normalize(diff);
    auto delta = diff * dist;
    // update position
    targetPosition_ += delta;
}

void Camera::follow(Player::Shared target)
{
    if (target_ && target_->id() == target->id()) {
        target_ = target;
        return;
    }
    target_ = target;
    targetPosition_ = target_->transform()->translation();
}

void Camera::setPerspective(float32_t fov, float32_t aspect, float32_t near, float32_t far)
{
    fov_ = fov;
    aspect_ = aspect;
    near_ = near;
    far_ = far;
}

void Camera::setAspect(float32_t aspect)
{
    aspect_ = aspect;
}

glm::mat4 Camera::projection() const
{
    return glm::perspective(fov_, aspect_, near_, far_);
}

void Camera::zoom(float32_t delta)
{
    zoomVelocity_ += delta * -SCROLL_FACTOR;
}

void Camera::rotate(glm::vec2 delta)
{
    rotationVelocity_.x += delta.x * ROTATE_X_FACTOR * (M_PI / 180.0);
    rotationVelocity_.y += delta.y * ROTATE_Y_FACTOR * (M_PI / 180.0);
}

Transform::Shared Camera::transform()
{
    return transform_;
}

glm::vec3 Camera::mouseToWorld(glm::vec2 position, float32_t width, float32_t height)
{
    auto mvp = projection() * transform_->viewMatrix();
    auto mvpInverse = glm::inverse(mvp);
    // map window coords to range [0 .. 1]
    auto nx = position.x / width;
    auto ny = (height - position.y) / height;
    auto nz = 0.0;
    // map to range of [-1 .. 1]
    auto input = glm::vec4(
        (nx * 2.0) - 1.0,
        (ny * 2.0) - 1.0,
        (nz * 2.0) - 1.0,
        1.0);
    auto output = mvpInverse * input;
    if (output.w == 0.0) {
        LOG_ERROR("w == 0.0");
        return glm::vec3();
    }
    auto world = glm::vec3(
        output.x / output.w,
        output.y / output.w,
        output.z / output.w);
    return glm::normalize(world - transform_->translation());
}
