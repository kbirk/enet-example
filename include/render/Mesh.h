#pragma once

#include "Common.h"
#include "gl/VertexArrayObject.h"
#include "render/Material.h"
#include "render/RenderCommand.h"
#include "render/Technique.h"

#include <memory>

class Mesh {

public:
    typedef std::shared_ptr<Mesh> Shared;
    static Shared alloc(
        VertexArrayObject::Shared,
        Material::Shared,
        Technique::Shared
        //Geometry::Shared
        );

    Mesh(
        VertexArrayObject::Shared,
        Material::Shared,
        Technique::Shared
        //Geometry::Shared
        );

    RenderCommand::Shared render() const;

private:
    // prevent copy-construction+
    Mesh(const Mesh&);
    // prevent assignment
    Mesh& operator=(const Mesh&);

    VertexArrayObject::Shared vao_;
    Material::Shared material_;
    Technique::Shared technique_;
    //Geometry::Shared geometry_;
};
