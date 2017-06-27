#pragma once

#include "Common.h"
#include "gl/ElementArrayBufferObject.h"
#include "gl/GLCommon.h"
#include "gl/VertexAttributePointer.h"
#include "gl/VertexBufferObject.h"

#include <memory>
#include <vector>

class VertexArrayObject {

public:
    typedef std::shared_ptr<VertexArrayObject> Shared;
    static Shared alloc();

    VertexArrayObject();
    ~VertexArrayObject();

    void attach(VertexBufferObject::Shared, VertexAttributePointer::Shared);
    void attach(ElementArrayBufferObject::Shared);
    void upload();

    void draw() const;

private:
    // prevent copy-construction
    VertexArrayObject(const VertexArrayObject&);
    // prevent assignment
    VertexArrayObject& operator=(const VertexArrayObject&);

    GLuint id_;
    std::vector<VertexBufferObject::Shared> vbos_;
    std::vector<VertexAttributePointer::Shared> pointers_;
    ElementArrayBufferObject::Shared eabo_;
};
