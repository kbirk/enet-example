#pragma once

#include "Common.h"
#include "gl/GLCommon.h"

#include <map>
#include <memory>
#include <string>

class UniformBlockDescriptor {

public:
    typedef std::shared_ptr<UniformBlockDescriptor> Shared;
    static Shared alloc(
        const std::string&,
        const GLint,
        const GLint,
        const std::map<std::string, GLint>&);

    UniformBlockDescriptor(
        const std::string&,
        const GLint,
        const GLint,
        const std::map<std::string, GLint>&);

    GLuint blockIndex() const;
    GLint alignedBlockSize() const;
    GLint unAlignedBlockSize() const;
    GLint offset(const std::string&) const;

private:
    // prevent copy-construction
    UniformBlockDescriptor(const UniformBlockDescriptor&);
    // prevent assignment
    UniformBlockDescriptor& operator=(const UniformBlockDescriptor&);

    std::string blockName_;
    GLuint blockIndex_;
    GLint blockSize_;
    std::map<std::string, GLint> offsets_;
};
