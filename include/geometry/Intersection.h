#pragma once

#include "Common.h"
#include "serial/StreamBuffer.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

class Intersection {

public:
    Intersection();
    Intersection(const glm::vec3&, const glm::vec3&, float32_t);

    bool hit;
    glm::vec3 position;
    glm::vec3 normal;
    float32_t t;

    friend StreamBuffer::Shared& operator<<(StreamBuffer::Shared& stream, const Intersection& geometry);
    friend StreamBuffer::Shared& operator>>(StreamBuffer::Shared& stream, Intersection& geometry);
};
