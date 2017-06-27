#include "geometry/Geometry.h"

Geometry::Shared Geometry::alloc()
{
    return std::make_shared<Geometry>();
}

Geometry::Geometry()
{
}

void Geometry::setPositions(const std::vector<glm::vec3>& positions)
{
    positions_ = positions;
}

void Geometry::setNormals(const std::vector<glm::vec3>& normals)
{
    normals_ = normals;
}

void Geometry::setUVs(const std::vector<glm::vec2>& uvs)
{
    uvs_ = uvs;
}

void Geometry::setWeights(const std::vector<glm::vec4>& weights)
{
    weights_ = weights;
}

void Geometry::setIndices(const std::vector<uint32_t>& indices)
{
    indices_ = indices;
}

const std::vector<glm::vec3>& Geometry::positions() const
{
    return positions_;
}

const std::vector<glm::vec3>& Geometry::normals() const
{
    return normals_;
}

const std::vector<glm::vec2>& Geometry::uvs() const
{
    return uvs_;
}

const std::vector<glm::vec4>& Geometry::weights() const
{
    return weights_;
}

const std::vector<uint32_t>& Geometry::indices() const
{
    return indices_;
}

void Geometry::generateOctree(uint8_t depth)
{
    auto triangles = std::vector<Triangle::Shared>();
    for (auto i = uint32_t(0); i < indices_.size(); i += 3) {
        auto a = positions_[indices_[i]];
        auto b = positions_[indices_[i + 1]];
        auto c = positions_[indices_[i + 2]];
        triangles.push_back(Triangle::alloc(a, b, c));
    }
    LOG_INFO("triangles: " << triangles.size());
    octree_ = Octree::alloc(triangles, depth);
}

Intersection Geometry::intersect(const glm::vec3& ray, const glm::vec3& origin, bool ignoreBehindRay, bool backFaceCull) const
{
    if (!octree_) {
        return Intersection();
    }
    return octree_->intersect(ray, origin, ignoreBehindRay, backFaceCull);
}

StreamBuffer::Shared& operator<<(StreamBuffer::Shared& stream, const Geometry::Shared& geometry)
{
    stream << geometry->positions_;
    stream << geometry->normals_;
    stream << geometry->uvs_;
    stream << geometry->weights_;
    stream << geometry->indices_;
    return stream;
}

StreamBuffer::Shared& operator>>(StreamBuffer::Shared& stream, Geometry::Shared& geometry)
{
    stream >> geometry->positions_;
    stream >> geometry->normals_;
    stream >> geometry->uvs_;
    stream >> geometry->weights_;
    stream >> geometry->indices_;
    return stream;
}
