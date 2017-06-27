#include "gl/Texture2D.h"

Texture2D::Shared Texture2D::alloc(
    GLuint internalFormat, GLuint format, GLuint type)
{
    return std::make_shared<Texture2D>(internalFormat, format, type);
}

Texture2D::Texture2D(GLuint internalFormat, GLuint format, GLuint type)
    : id_(0)
    , width_(0)
    , height_(0)
    , internalFormat_(internalFormat)
    , format_(format)
    , type_(type)
    , parameters_({ { GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR },
          { GL_TEXTURE_MAG_FILTER, GL_LINEAR },
          { GL_TEXTURE_WRAP_S, GL_REPEAT },
          { GL_TEXTURE_WRAP_T, GL_REPEAT } })
{
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &id_);
    LOG_OPENGL("glDeleteTextures");
    id_ = 0;
}

GLuint Texture2D::width() const
{
    return width_;
}

GLuint Texture2D::height() const
{
    return height_;
}

void Texture2D::bind(GLenum textureUnit) const
{
    glActiveTexture(textureUnit);
    LOG_OPENGL("glActiveTexture");
    // bind textures AFTER binding shader AND BEFORE drawing arrays
    glBindTexture(GL_TEXTURE_2D, id_);
    LOG_OPENGL("glBindTexture");
}

void Texture2D::unbind() const
{
    // unbind textures after drawing
    glBindTexture(GL_TEXTURE_2D, 0);
    LOG_OPENGL("glBindTexture");
}

void Texture2D::setParameters(const std::map<GLuint, GLuint>& params)
{
    for (auto& param : params) {
        parameters_[param.first] = param.second;
    }
    glBindTexture(GL_TEXTURE_2D, id_);
    LOG_OPENGL("glBindTexture");
    for (auto parameter : parameters_) {
        glTexParameterf(GL_TEXTURE_2D, parameter.first, parameter.second);
        LOG_OPENGL("glTexParameterf");
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    LOG_OPENGL("glBindTexture");
}

void Texture2D::generateMipmaps()
{
    // generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);
    LOG_OPENGL("glGenerateMipmap");
}
