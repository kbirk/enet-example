#include "gl/TextureCubeMap.h"

TextureCubeMap::Shared TextureCubeMap::alloc(
    GLuint internalFormat, GLuint format, GLuint type)
{
    return std::make_shared<TextureCubeMap>(internalFormat, format, type);
}

TextureCubeMap::TextureCubeMap(GLuint internalFormat, GLuint format, GLuint type)
    : id_(0)
    , width_(0)
    , height_(0)
    , internalFormat_(internalFormat)
    , format_(format)
    , type_(type)
    , parameters_({ { GL_TEXTURE_MIN_FILTER, GL_LINEAR },
          { GL_TEXTURE_MAG_FILTER, GL_LINEAR },
          { GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },
          { GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE } })
{
}

TextureCubeMap::~TextureCubeMap()
{
    glDeleteTextures(1, &id_);
    LOG_OPENGL("glDeleteTextures");
    id_ = 0;
}

GLuint TextureCubeMap::width() const
{
    return width_;
}

GLuint TextureCubeMap::height() const
{
    return height_;
}

void TextureCubeMap::bind(GLenum textureUnit) const
{
    glActiveTexture(textureUnit);
    LOG_OPENGL("glActiveTexture");
    // bind textures AFTER binding shader AND BEFORE drawing arrays
    glBindTexture(GL_TEXTURE_CUBE_MAP, id_);
    LOG_OPENGL("glBindTexture");
}

void TextureCubeMap::unbind() const
{
    // unbind textures after drawing
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    LOG_OPENGL("glBindTexture");
}

void TextureCubeMap::setParameters(const std::map<GLuint, GLuint>& params)
{
    for (auto& param : params) {
        parameters_[param.first] = param.second;
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, id_);
    LOG_OPENGL("glBindTexture");
    for (auto parameter : parameters_) {
        glTexParameterf(GL_TEXTURE_CUBE_MAP, parameter.first, parameter.second);
        LOG_OPENGL("glTexParameterf");
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    LOG_OPENGL("glBindTexture");
}

void TextureCubeMap::generateMipmaps()
{
    // generate mipmaps
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    LOG_OPENGL("glGenerateMipmap");
}
