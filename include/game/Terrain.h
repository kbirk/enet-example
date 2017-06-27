#pragma once

#include "Common.h"
#include "geometry/Geometry.h"
#include "geometry/Geometry.h"
#include "gl/Texture2D.h"
#include "gl/VertexArrayObject.h"
#include "math/Transform.h"
#include "serial/StreamBuffer.h"

class Terrain {

public:
    typedef std::shared_ptr<Terrain> Shared;
    static Shared alloc();
    static Shared alloc(
        const std::string&,
        const std::string&,
        const std::string&,
        const std::string&);

    Terrain();
    Terrain(
        const std::string&,
        const std::string&,
        const std::string&,
        const std::string&);

    void generateGeometry(
        uint32_t cols,
        uint32_t rows,
        float32_t cellWidth,
        float32_t cellHeight,
        float32_t uv);

    void generateVAO();

    Transform::Shared transform();
    Texture2D::Shared texture(uint8_t index) const;
    VertexArrayObject::Shared vao() const;

    Intersection intersect(const glm::vec3&, const glm::vec3&, bool ignoreBehindRay = true, bool backFaceCull = true) const;

    friend StreamBuffer::Shared& operator<<(StreamBuffer::Shared&, const Terrain::Shared&);
    friend StreamBuffer::Shared& operator>>(StreamBuffer::Shared&, Terrain::Shared&);

private:
    // prevent copy-construction
    Terrain(const Terrain&);
    // prevent assignment
    Terrain& operator=(const Terrain&);

    Transform::Shared transform_;
    std::vector<Texture2D::Shared> textures_;
    Geometry::Shared geometry_;
    VertexArrayObject::Shared vao_;
};
