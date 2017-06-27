#pragma once

#include "Common.h"
#include "gl/GLCommon.h"

#include <map>
#include <memory>
#include <vector>

class TextureCubeMap {

public:
    typedef std::shared_ptr<TextureCubeMap> Shared;
    static Shared alloc(
        GLuint internalFormat = GL_RGBA,
        GLuint format = GL_RGBA,
        GLuint type = GL_UNSIGNED_BYTE);

    explicit TextureCubeMap(
        GLuint internalFormat = GL_RGBA,
        GLuint format = GL_RGBA,
        GLuint type = GL_UNSIGNED_BYTE);
    ~TextureCubeMap();

    void bind(GLenum textureUnit) const;
    void unbind() const;

    GLuint width() const;
    GLuint height() const;

    template <typename T>
    void upload(GLuint, GLuint, GLuint, GLuint, GLuint, const std::map<GLenum, std::vector<T> >&, bool = false);
    template <typename T>
    void upload(GLuint, GLuint, const std::map<GLenum, std::vector<T> >&, bool = false);

    void setParameters(const std::map<GLuint, GLuint>&);
    void generateMipmaps();

private:
    // prevent copy-construction
    TextureCubeMap(const TextureCubeMap&);
    // prevent assignment
    TextureCubeMap& operator=(const TextureCubeMap&);

    GLuint id_;
    GLuint width_;
    GLuint height_;
    GLuint internalFormat_;
    GLuint format_;
    GLuint type_;
    std::map<GLuint, GLuint> parameters_;
};

template <typename T>
void TextureCubeMap::upload(GLuint internalFormat, GLuint width, GLuint height, GLuint format, GLuint type, const std::map<GLenum, std::vector<T> >& data, bool generateMipmaps)
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
    glBindTexture(GL_TEXTURE_CUBE_MAP, id_);
    LOG_OPENGL("glBindTexture");
    for (auto iter : data) {
        auto face = iter.first;
        auto data = iter.second;
        // buffer
        if (data.size() > 0) {
            glTexImage2D(face, 0, internalFormat_, width_, height_, 0, format_, type_, static_cast<const GLvoid*>(&data[0]));
        } else {
            glTexImage2D(face, 0, internalFormat_, width_, height_, 0, format_, type_, 0);
        }
        LOG_OPENGL("glTexImage2D");
    }

    for (auto parameter : parameters_) {
        glTexParameterf(GL_TEXTURE_CUBE_MAP, parameter.first, parameter.second);
        LOG_OPENGL("glTexParameterf");
    }
    if (generateMipmaps) {
        // generate mipmaps
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        LOG_OPENGL("glGenerateMipmap");
    }
    // unbind
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    LOG_OPENGL("glBindTexture");
}

template <typename T>
void TextureCubeMap::upload(GLuint width, GLuint height, const std::map<GLenum, std::vector<T> >& data, bool generateMipmaps)
{
    upload(internalFormat_, width, height, format_, type_, data, generateMipmaps);
}
