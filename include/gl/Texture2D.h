#pragma once

#include "Common.h"
#include "gl/GLCommon.h"

#include <map>
#include <memory>
#include <vector>

class Texture2D {

public:
    typedef std::shared_ptr<Texture2D> Shared;
    static Shared alloc(
        GLuint internalFormat = GL_RGBA,
        GLuint format = GL_RGBA,
        GLuint type = GL_UNSIGNED_BYTE);

    explicit Texture2D(
        GLuint internalFormat = GL_RGBA,
        GLuint format = GL_RGBA,
        GLuint type = GL_UNSIGNED_BYTE);
    ~Texture2D();

    void bind(GLenum textureUnit) const;
    void unbind() const;

    GLuint width() const;
    GLuint height() const;

    template <typename T>
    void upload(GLuint, GLuint, GLuint, GLuint, GLuint, const std::vector<T>&, bool = true);
    template <typename T>
    void upload(GLuint, GLuint, const std::vector<T>&, bool = true);

    template <typename T>
    void uploadSub(GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, const std::vector<T>&, bool = false);
    template <typename T>
    void uploadSub(GLuint, GLuint, GLuint, GLuint, const std::vector<T>&, bool = false);

    void setParameters(const std::map<GLuint, GLuint>&);
    void generateMipmaps();

private:
    // prevent copy-construction
    Texture2D(const Texture2D&);
    // prevent assignment
    Texture2D& operator=(const Texture2D&);

    GLuint id_;
    GLuint width_;
    GLuint height_;
    GLuint internalFormat_;
    GLuint format_;
    GLuint type_;
    std::map<GLuint, GLuint> parameters_;
};

template <typename T>
void Texture2D::upload(GLuint internalFormat, GLuint width, GLuint height, GLuint format, GLuint type, const std::vector<T>& data, bool generateMipmaps)
{
    if (!id_) {
        glGenTextures(1, &id_);
        LOG_OPENGL("glGenTextures");
    }
    internalFormat_ = internalFormat;
    width_ = width;
    height_ = height;
    format_ = format;
    type_ = type;
    // bind
    glBindTexture(GL_TEXTURE_2D, id_);
    LOG_OPENGL("glBindTexture");
    // buffer
    if (data.size() > 0) {
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat_, width_, height_, 0, format_, type_, static_cast<const GLvoid*>(&data[0]));
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat_, width_, height_, 0, format_, type_, 0);
    }
    LOG_OPENGL("glTexImage2D");
    for (auto parameter : parameters_) {
        glTexParameterf(GL_TEXTURE_2D, parameter.first, parameter.second);
        LOG_OPENGL("glTexParameterf");
    }
    if (generateMipmaps) {
        // generate mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);
        LOG_OPENGL("glGenerateMipmap");
    }
    // unbind
    glBindTexture(GL_TEXTURE_2D, 0);
    LOG_OPENGL("glBindTexture");
}

template <typename T>
void Texture2D::upload(GLuint width, GLuint height, const std::vector<T>& data, bool generateMipmaps)
{
    upload(internalFormat_, width, height, format_, type_, data, generateMipmaps);
}

template <typename T>
void Texture2D::uploadSub(GLuint xOffset, GLuint yOffset, GLuint width, GLuint height, GLuint format, GLuint type, const std::vector<T>& data, bool generateMipmaps)
{
    format_ = format;
    type_ = type;
    // bind
    glBindTexture(GL_TEXTURE_2D, id_);
    LOG_OPENGL("glBindTexture");
    // buffer
    glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, width, height, format_, type_, static_cast<const GLvoid*>(&data[0]));
    LOG_OPENGL("glTexSubImage2D");
    // generate mipmaps
    if (generateMipmaps) {
        // generate mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);
        LOG_OPENGL("glGenerateMipmap");
    }
    // unbind
    glBindTexture(GL_TEXTURE_2D, 0);
    LOG_OPENGL("glBindTexture");
}

template <typename T>
void Texture2D::uploadSub(GLuint xOffset, GLuint yOffset, GLuint width, GLuint height, const std::vector<T>& data, bool generateMipmaps)
{
    uploadSub(xOffset, yOffset, width, height, format_, type_, data, generateMipmaps);
}
