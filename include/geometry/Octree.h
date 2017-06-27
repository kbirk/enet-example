#pragma once

#include "Common.h"
#include "geometry/Triangle.h"
#include "gl/VertexArrayObject.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

// children quadrant indices:
//   [0] = - - -
//   [1] = + - -
//   [2] = - + -
//   [3] = + + -
//   [4] = - - +
//   [5] = + - +
//   [6] = - + +
//   [7] = + + +

class Octree {

public:
    typedef std::shared_ptr<Octree> Shared;
    static Shared alloc();
    static Shared alloc(const glm::vec3&, float32_t, uint32_t);
    static Shared alloc(const std::vector<Triangle::Shared>&, uint32_t);

    Octree();
    Octree(const glm::vec3&, float32_t, uint32_t);
    Octree(const std::vector<Triangle::Shared>&, uint32_t);

    ~Octree();

    Intersection intersect(
        const glm::vec3&,
        const glm::vec3&,
        bool ignoreBehindRay = true,
        bool backFaceCull = false) const;

private:
    // prevent copy-construction
    Octree(const Octree&);
    // prevent assignment
    Octree& operator=(const Octree&);

    void build(
        const glm::vec3& min,
        const glm::vec3& max,
        const std::vector<Triangle::Shared>& triangles,
        uint32_t depth);
    void clear();
    void insert(Triangle::Shared tri);
    void insertIntoChild(uint32_t index, Triangle::Shared tri);
    float32_t sqrDistFromPoint(const glm::vec3& point, int32_t child) const;
    glm::vec3 closestPointInBox(const glm::vec3& point, int32_t child) const;
    bool sphereCheck(const glm::vec3& centre, float32_t radius, int32_t child) const;
    VertexArrayObject::Shared generateVAO(float32_t halfWidth) const;

    uint32_t depth_;
    glm::vec3 center_;
    float32_t halfWidth_;
    std::vector<Octree::Shared> children_;
    std::vector<Triangle::Shared> contained_;
};
