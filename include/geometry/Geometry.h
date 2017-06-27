#pragma once

#include "Common.h"
#include "geometry/Intersection.h"
#include "geometry/Octree.h"
#include "serial/StreamBuffer.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

class Geometry {

public:
    typedef std::shared_ptr<Geometry> Shared;
    static Shared alloc();

    Geometry();

    void setPositions(const std::vector<glm::vec3>&);
    void setNormals(const std::vector<glm::vec3>&);
    void setUVs(const std::vector<glm::vec2>&);
    void setWeights(const std::vector<glm::vec4>&);
    void setIndices(const std::vector<uint32_t>&);

    const std::vector<glm::vec3>& positions() const;
    const std::vector<glm::vec3>& normals() const;
    const std::vector<glm::vec2>& uvs() const;
    const std::vector<glm::vec4>& weights() const;
    const std::vector<uint32_t>& indices() const;

    void generateOctree(uint8_t = 5);

    Intersection intersect(const glm::vec3&, const glm::vec3&, bool ignoreBehindRay = true, bool backFaceCull = true) const;

    friend StreamBuffer::Shared& operator<<(StreamBuffer::Shared& stream, const Geometry::Shared& geometry);
    friend StreamBuffer::Shared& operator>>(StreamBuffer::Shared& stream, Geometry::Shared& geometry);

private:
    // prevent copy-construction
    Geometry(const Geometry&);
    // prevent assignment
    Geometry& operator=(const Geometry&);

    std::vector<glm::vec3> positions_;
    std::vector<glm::vec3> normals_;
    std::vector<glm::vec2> uvs_;
    std::vector<glm::vec4> weights_;
    std::vector<uint32_t> indices_;
    Octree::Shared octree_;
};
