#include "gl/UniformDescriptor.h"

UniformDescriptor::Shared UniformDescriptor::alloc(
    const std::string& name,
    GLenum type,
    GLint location)
{
    return std::make_shared<UniformDescriptor>(name, type, location);
}

UniformDescriptor::UniformDescriptor(
    const std::string& name,
    GLenum type,
    GLint location)
    : name_(name)
    , type_(type)
    , location_(location)
{
}

GLenum UniformDescriptor::type() const
{
    return type_;
}

GLint UniformDescriptor::location() const
{
    return location_;
}
