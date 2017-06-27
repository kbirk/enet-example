#include "render/Material.h"

Material::Shared Material::alloc()
{
    return std::make_shared<Material>();
}

Material::Material()
{
}

void Material::addUniform(const std::string& name, Uniform::Shared uniform)
{
    uniforms_[name] = uniform;
}

void Material::addTexture(GLenum location, Texture2D::Shared texture)
{
    textures_[location] = texture;
}

RenderCommand::Shared Material::render() const
{
    auto command = RenderCommand::alloc();
    for (auto iter : uniforms_) {
        command->uniforms[iter.first] = iter.second;
    }
    for (auto iter : textures_) {
        command->textures[iter.first] = iter.second;
    }
    return command;
}
