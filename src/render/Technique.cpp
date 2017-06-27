#include "render/Technique.h"

Technique::Shared Technique::alloc()
{
    return std::make_shared<Technique>();
}

Technique::Technique()
{
}

void Technique::setShader(Shader::Shared shader)
{
    shader_ = shader;
}

void Technique::setUniform(const std::string& name, Uniform::Shared uniform)
{
    uniforms_[name] = uniform;
}

void Technique::setFunction(FunctionType type, const std::vector<GLenum>& values)
{
    functions_[type] = values;
}

void Technique::setViewport(Viewport::Shared viewport)
{
    viewport_ = viewport;
}

void Technique::enable(GLenum enable)
{
    enables_.push_back(enable);
}

RenderCommand::Shared Technique::render() const
{
    auto command = RenderCommand::alloc();
    command->shader = shader_;
    command->uniforms = uniforms_;
    command->functions = functions_;
    command->enables = enables_;
    return command;
}
