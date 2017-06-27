#include "gl/VertexBufferObject.h"

VertexBufferObject::Shared VertexBufferObject::alloc(
    GLenum mode,
    GLuint count,
    GLuint byteOffset)
{
    return std::make_shared<VertexBufferObject>(mode, count, byteOffset);
}

VertexBufferObject::VertexBufferObject(
    GLenum mode,
    GLuint count,
    GLuint byteOffset)
    : id_(0)
    , mode_(mode)
    , count_(count)
    , byteOffset_(byteOffset)
{
}

VertexBufferObject::~VertexBufferObject()
{
    if (id_ != 0) {
        glDeleteBuffers(1, &id_);
        LOG_OPENGL("glDeleteBuffers");
        id_ = 0;
    }
}

void VertexBufferObject::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, id_);
    LOG_OPENGL("glBindBuffer");
}

void VertexBufferObject::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    LOG_OPENGL("glBindBuffer");
}

void VertexBufferObject::draw() const
{
    glDrawArrays(mode_, byteOffset_, count_);
    LOG_OPENGL("glDrawArrays");
}
