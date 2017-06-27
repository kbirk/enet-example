#pragma once

#include "Common.h"
#include "gl/Texture2D.h"
#include "gl/Uniform.h"
#include "render/RenderCommand.h"

#include <map>
#include <memory>
#include <string>

class Material {

public:
    typedef std::shared_ptr<Material> Shared;
    static Shared alloc();

    Material();

    void addUniform(const std::string&, Uniform::Shared);
    void addTexture(GLenum, Texture2D::Shared);

    RenderCommand::Shared render() const;

private:
    // prevent copy-construction
    Material(const Material&);
    // prevent assignment
    Material& operator=(const Material&);

    std::map<std::string, Uniform::Shared> uniforms_;
    std::map<GLenum, Texture2D::Shared> textures_;
};
