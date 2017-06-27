#include "gl/VertexAttributePointer.h"

VertexAttributePointer::Shared VertexAttributePointer::alloc(
    GLuint index,
    GLuint size,
    GLuint type,
    GLuint byteOffset,
    GLuint stride)
{
    return std::make_shared<VertexAttributePointer>(
        index,
        size,
        type,
        byteOffset,
        stride);
}

VertexAttributePointer::VertexAttributePointer(
    GLuint index,
    GLuint size,
    GLuint type,
    GLuint byteOffset,
    GLuint stride)
    : index_(index)
    , size_(size)
    , type_(type)
    , byteOffset_(byteOffset)
    , stride_(stride)
{
}

void VertexAttributePointer::bind() const
{
    glEnableVertexAttribArray(index_);
    LOG_OPENGL("glEnableVertexAttribArray");
    glVertexAttribPointer(
        index_,
        size_,
        type_,
        GL_FALSE,
        stride_,
        (GLbyte*)(nullptr) + (byteOffset_));
    LOG_OPENGL("glVertexAttribPointer");
}

void VertexAttributePointer::unbind() const
{
    glDisableVertexAttribArray(index_);
    LOG_OPENGL("glDisableVertexAttribArray");
}
