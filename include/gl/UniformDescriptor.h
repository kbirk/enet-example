#pragma once

#include "Common.h"
#include "gl/GLCommon.h"

#include <memory>
#include <string>

class UniformDescriptor {

public:
    typedef std::shared_ptr<UniformDescriptor> Shared;
    static Shared alloc(const std::string&, GLenum, GLint);

    UniformDescriptor(const std::string&, GLenum, GLint);

    GLenum type() const;
    GLint location() const;

private:
    // prevent copy-construction
    UniformDescriptor(const UniformDescriptor&);
    // prevent assignment
    UniformDescriptor& operator=(const UniformDescriptor&);

    std::string name_;
    GLenum type_;
    GLint location_;
};
