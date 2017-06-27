#include "gl/ElementArrayBufferObject.h"

ElementArrayBufferObject::Shared ElementArrayBufferObject::alloc(
    GLenum type,
    GLenum mode,
    GLuint count,
    GLuint byteOffset)
{
    return std::make_shared<ElementArrayBufferObject>(
        type,
        mode,
        count,
        byteOffset);
}

ElementArrayBufferObject::~ElementArrayBufferObject()
{
    if (id_ != 0) {
        glDeleteBuffers(1, &id_);
        LOG_OPENGL("glDeleteBuffers");
        id_ = 0;
    }
}

ElementArrayBufferObject::ElementArrayBufferObject(
    GLenum type,
    GLenum mode,
    GLuint count,
    GLuint byteOffset)
    : id_(0)
    , type_(type)
    , mode_(mode)
    , count_(count)
    , byteOffset_(byteOffset)
{
}

void ElementArrayBufferObject::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
    LOG_OPENGL("glBindBuffer");
}

void ElementArrayBufferObject::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    LOG_OPENGL("glBindBuffer");
}

void ElementArrayBufferObject::draw() const
{
    glDrawElements(
        mode_,
        count_,
        type_,
        (GLbyte*)(nullptr) + (byteOffset_));
    LOG_OPENGL("glDrawElements");
}
