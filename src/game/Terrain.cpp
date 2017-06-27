#include "game/Terrain.h"

#include "game/Image.h"

#include "Simplex.h"

Texture2D::Shared loadTextureRGBA(const std::string& path)
{
    // load image
    uint32_t width, height;
    // load and invert image y
    std::vector<uint8_t> image = loadImage(width, height, path, true);
    // create texture
    auto texture = Texture2D::alloc(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    texture->upload(width, height, image, true);
    return texture;
}

glm::vec4 getWeights(float32_t n)
{
    auto third = 1.0 / 3.0;
    auto p = fmod(n, third) / third;
    if (n < third) {
        return glm::vec4(1 - p, p, 0, 0);
    } else if (n < third * 2) {
        return glm::vec4(0, 1 - p, p, 0);
    }
    return glm::vec4(0, 0, 1 - p, p);
}

Terrain::Shared Terrain::alloc()
{
    return std::make_shared<Terrain>();
}

Terrain::Shared Terrain::alloc(
    const std::string& file0,
    const std::string& file1,
    const std::string& file2,
    const std::string& file3)
{
    return std::make_shared<Terrain>(file0, file1, file2, file3);
}

Terrain::Terrain()
{
    transform_ = Transform::alloc();
}

Terrain::Terrain(
    const std::string& file0,
    const std::string& file1,
    const std::string& file2,
    const std::string& file3)
{
    textures_.push_back(loadTextureRGBA(file3));
    textures_.push_back(loadTextureRGBA(file2));
    textures_.push_back(loadTextureRGBA(file1));
    textures_.push_back(loadTextureRGBA(file0));
    transform_ = Transform::alloc();
}

void Terrain::generateGeometry(uint32_t cols, uint32_t rows, float32_t width, float32_t height, float32_t uv)
{

    auto flip = true;
    auto size = (rows + 1) * (cols + 1);

    auto positions = std::vector<glm::vec3>(size);
    auto normals = std::vector<glm::vec3>(size);
    auto uvs = std::vector<glm::vec2>(size);
    auto weights = std::vector<glm::vec4>(size);

    // set positions / weights

    auto frows = float32_t(rows);
    auto fcols = float32_t(cols);
    float32_t min = 1.0;
    float32_t max = 0.0;
    auto index = 0;

    for (float32_t i = -frows / 2; i <= frows / 2; i++) {
        for (float32_t j = -fcols / 2; j <= fcols / 2; j++) {

            auto n = Simplex::fBm(glm::vec2(i / frows, j / fcols));

            n = (n + 1.0) * 0.5;

            min = std::min(min, n);
            max = std::max(max, n);

            positions[index] = glm::vec3(
                i * width,
                height * n,
                j * width);

            weights[index] = getWeights(n);

            index++;
        }
    }

    // uvs / normals / indices

    auto count = rows * cols * 6;

    auto indices = std::vector<uint32_t>(count);

    for (auto i = 0; i < int32_t(rows); i++) {
        for (auto j = 0; j < int32_t(cols); j++) {

            auto index = i * cols + j;

            auto i0 = uint32_t(j + i * (cols + 1));
            auto i1 = uint32_t(j + 1 + i * (cols + 1));
            auto i2 = uint32_t(j + (cols + 2) + i * (cols + 1));
            auto i3 = uint32_t(j + (cols + 1) + i * (cols + 1));

            uint32_t a, b, c, d, e, f;

            if (flip) {

                a = i0;
                b = i1;
                c = i2;

                d = i0;
                e = i2;
                f = i3;

            } else {

                a = i0;
                b = i1;
                c = i3;

                d = i1;
                e = i2;
                f = i3;
            }

            flip = !flip;

            // set indices
            indices[index * 6] = a;
            indices[index * 6 + 1] = b;
            indices[index * 6 + 2] = c;
            indices[index * 6 + 3] = d;
            indices[index * 6 + 4] = e;
            indices[index * 6 + 5] = f;

            auto va = positions[a];
            auto vb = positions[b];
            auto vc = positions[c];
            auto vd = positions[d];
            auto ve = positions[e];
            auto vf = positions[f];

            // calc normals
            auto n0 = cross((vc - vb), (va - vb));
            auto n1 = cross((vf - ve), (vd - ve));

            // add normal weights (for smoothing)
            normals[a] = normals[a] + n0;
            normals[b] = normals[b] + n0;
            normals[c] = normals[c] + n0;
            normals[d] = normals[d] + n1;
            normals[e] = normals[e] + n1;
            normals[f] = normals[f] + n1;

            // set uvs
            uvs[i0] = glm::vec2(j * uv, i * uv);
            uvs[i1] = glm::vec2((j + 1) * uv, i * uv);
            uvs[i2] = glm::vec2((j + 1) * uv, (i + 1) * uv);
            uvs[i3] = glm::vec2(j * uv, (i + 1) * uv);
        }
    }

    // normalize normals (smoothing)
    for (auto i = 0; i < int32_t(size); i++) {
        normals[i] = glm::normalize(normals[i]);
    }

    // create geometry
    geometry_ = Geometry::alloc();
    geometry_->setPositions(positions);
    geometry_->setNormals(normals);
    geometry_->setUVs(uvs);
    geometry_->setWeights(weights);
    geometry_->setIndices(indices);
    geometry_->generateOctree();

    LOG_INFO("num positions: " << geometry_->positions().size());
    LOG_INFO("num indices: " << geometry_->indices().size());
}

void Terrain::generateVAO()
{
    if (!geometry_) {
        return;
    }
    // positions
    auto positions = VertexBufferObject::alloc();
    positions->upload(geometry_->positions());
    // normals
    auto normals = VertexBufferObject::alloc();
    normals->upload(geometry_->normals());
    // uvs
    auto uvs = VertexBufferObject::alloc();
    uvs->upload(geometry_->uvs());
    // uvs
    auto weights = VertexBufferObject::alloc();
    weights->upload(geometry_->weights());
    // indices
    auto indices = ElementArrayBufferObject::alloc();
    indices->upload(geometry_->indices());
    // vao
    vao_ = VertexArrayObject::alloc();
    vao_->attach(positions, VertexAttributePointer::alloc(0, 3, GL_FLOAT));
    vao_->attach(normals, VertexAttributePointer::alloc(1, 3, GL_FLOAT));
    vao_->attach(uvs, VertexAttributePointer::alloc(2, 2, GL_FLOAT));
    vao_->attach(weights, VertexAttributePointer::alloc(3, 4, GL_FLOAT));
    vao_->attach(indices);
    vao_->upload();
}

Transform::Shared Terrain::transform()
{
    return transform_;
}

Texture2D::Shared Terrain::texture(uint8_t index) const
{
    return textures_[index];
}

VertexArrayObject::Shared Terrain::vao() const
{
    return vao_;
}

Intersection Terrain::intersect(const glm::vec3& ray, const glm::vec3& origin, bool ignoreBehindRay, bool backFaceCull) const
{
    if (geometry_) {
        auto matrix = transform_->matrix();
        auto inv = glm::inverse(matrix);
        auto transformedRay = glm::normalize(glm::vec3(inv * glm::vec4(ray, 0.0)));
        auto transformedOrigin = glm::vec3(inv * glm::vec4(origin, 1.0));
        auto intersection = geometry_->intersect(
            transformedRay,
            transformedOrigin,
            ignoreBehindRay,
            backFaceCull);
        if (intersection.hit) {
            auto transformedPos = glm::vec3(matrix * glm::vec4(intersection.position, 1.0));
            auto transformedNorm = glm::normalize(glm::vec3(matrix * glm::vec4(intersection.normal, 0.0)));
            intersection.position = transformedPos;
            intersection.normal = transformedNorm;
        }
        return intersection;
    }
    return Intersection();
}

StreamBuffer::Shared& operator<<(StreamBuffer::Shared& stream, const Terrain::Shared& terrain)
{
    if (terrain->geometry_) {
        stream << terrain->geometry_;
    }
    return stream;
}

StreamBuffer::Shared& operator>>(StreamBuffer::Shared& stream, Terrain::Shared& terrain)
{
    stream >> terrain->geometry_;
    return stream;
}
