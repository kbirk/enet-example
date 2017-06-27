#pragma once

#include "Common.h"
#include "gl/GLCommon.h"

#include <memory>
#include <vector>

class Uniform {

public:
    typedef std::shared_ptr<Uniform> Shared;

    template <typename T>
    static Shared alloc(const T&);

    template <typename T>
    static Shared alloc(const std::vector<T>&);

    template <typename T>
    explicit Uniform(const T&);

    template <typename T>
    explicit Uniform(const std::vector<T>&);

    const GLubyte* data() const;
    GLuint numBytes() const;
    GLsizei count() const;

private:
    // prevent copy-construction
    Uniform(const Uniform&);
    // prevent assignment
    Uniform& operator=(const Uniform&);

    std::vector<GLubyte> data_;
    GLsizei count_;
};

template <typename T>
Uniform::Shared Uniform::alloc(const T& t)
{
    return std::make_shared<Uniform>(t);
}

template <typename T>
Uniform::Shared Uniform::alloc(const std::vector<T>& t)
{
    return std::make_shared<Uniform>(t);
}

template <typename T>
Uniform::Uniform(const T& t)
    : count_(1)
{
    uint32_t numBytes = sizeof(T);
    data_.resize(numBytes);
    memcpy(&data_[0], reinterpret_cast<const GLubyte*>(&t), numBytes);
}

template <typename T>
Uniform::Uniform(const std::vector<T>& t)
    : count_(t.size())
{
    uint32_t numBytes = t.size() * sizeof(T);
    data_.resize(numBytes);
    memcpy(&data_[0], reinterpret_cast<const GLubyte*>(&t[0]), numBytes);
}
