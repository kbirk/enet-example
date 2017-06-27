#pragma once

#include "gl/GLCommon.h"
#include "gl/GLInfo.h"

#include <memory>
#include <vector>

class ElementArrayBufferObject {

public:
    typedef std::shared_ptr<ElementArrayBufferObject> Shared;
    static Shared alloc(
        GLenum type = GL_UNSIGNED_INT,
        GLenum mode = GL_TRIANGLES,
        GLuint count = 0,
        GLuint byteOffset = 0);

    explicit ElementArrayBufferObject(
        GLenum type = GL_UNSIGNED_INT,
        GLenum mode = GL_TRIANGLES,
        GLuint count = 0,
        GLuint byteOffset = 0);

    ~ElementArrayBufferObject();

    template <typename T>
    void upload(const std::vector<T>&, GLenum usage = GL_STATIC_DRAW);

    void bind() const;
    void unbind() const;
    void draw() const;

private:
    // prevent copy-construction
    ElementArrayBufferObject(const ElementArrayBufferObject&);
    // prevent assignment
    ElementArrayBufferObject& operator=(const ElementArrayBufferObject&);

    GLuint id_;
    GLenum type_;
    GLenum mode_;
    GLuint count_;
    GLuint byteOffset_;
};

template <typename T>
void ElementArrayBufferObject::upload(const std::vector<T>& data, GLenum usage)
{
    // if buffer not allocated, generate
    if (!id_) {
        glGenBuffers(1, &id_);
        LOG_OPENGL("glGenBuffers");
    }
    // get buffer size
    const size_t bufferSize = data.size();
    // update count if it is zero
    if (count_ == 0) {
        count_ = bufferSize;
    }
    // bind buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
    LOG_OPENGL("glBindBuffer");
    // buffer the data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize * sizeof(T), &data[0], usage);
    LOG_OPENGL("glBufferData");
    // unbind the buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    LOG_OPENGL("glBindBuffer");
}
