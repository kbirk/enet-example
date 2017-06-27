#include "gl/VertexFragmentShader.h"

VertexFragmentShader::Shared VertexFragmentShader::alloc()
{
    return std::make_shared<VertexFragmentShader>();
}

VertexFragmentShader::VertexFragmentShader()
{
}

bool VertexFragmentShader::create(const std::string& vert, const std::string& frag)
{
    // create and compile shader, tests for errors
    GLuint vertexShader = createAndCompileShader(vert, GL_VERTEX_SHADER);
    if (vertexShader == 0) {
        return false;
    }
    GLuint fragmentShader = createAndCompileShader(frag, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        return false;
    }
    // create program
    id_ = glCreateProgram();
    LOG_OPENGL("glCreateProgram");
    // attach shaders
    glAttachShader(id_, vertexShader);
    LOG_OPENGL("glAttachShader");
    glAttachShader(id_, fragmentShader);
    LOG_OPENGL("glAttachShader");
    // link program, check for error
    bool error = false;
    if (!linkProgram()) {
        glDetachShader(id_, vertexShader);
        LOG_OPENGL("glDetachShader");
        glDetachShader(id_, fragmentShader);
        LOG_OPENGL("glDetachShader");
        // flag as error
        error = true;
    }
    // free shader objects
    glDeleteShader(vertexShader);
    LOG_OPENGL("glDeleteShader");
    glDeleteShader(fragmentShader);
    LOG_OPENGL("glDeleteShader");
    // exit if error linking
    if (error) {
        return false;
    }
    // query uniform information
    queryUniforms();
    return true;
}

void VertexFragmentShader::setUniform(const std::string& name, const Uniform::Shared& uniform, bool ignore)
{
    // check descriptors
    const auto& iter = descriptors_.find(name);
    if (iter == descriptors_.end()) {
        if (!ignore) {
            LOG_ERROR("Uniform `" << name << "` was not recognized");
        }
        return;
    }
    // get descriptor
    const auto& descriptor = iter->second;
    GLint location = descriptor->location();
    GLint type = descriptor->type();
    // buffer uniform data
    switch (type) {
    case GL_SAMPLER_2D:
    case GL_SAMPLER_CUBE:
    case GL_INT:
        glUniform1iv(
            location,
            uniform->count(),
            reinterpret_cast<const GLint*>(uniform->data()));
        break;
    case GL_UNSIGNED_INT:
    case GL_BOOL:
        glUniform1uiv(
            location,
            uniform->count(),
            reinterpret_cast<const GLuint*>(uniform->data()));
        break;
    case GL_FLOAT:
        glUniform1fv(
            location,
            uniform->count(),
            reinterpret_cast<const GLfloat*>(uniform->data()));
        break;
    case GL_INT_VEC2:
        glUniform2iv(
            location,
            uniform->count(),
            reinterpret_cast<const GLint*>(uniform->data()));
        break;
    case GL_INT_VEC3:
        glUniform3iv(
            location,
            uniform->count(),
            reinterpret_cast<const GLint*>(uniform->data()));
        break;
    case GL_INT_VEC4:
        glUniform4iv(
            location,
            uniform->count(),
            reinterpret_cast<const GLint*>(uniform->data()));
        break;
    case GL_FLOAT_VEC2:
        glUniform2fv(
            location,
            uniform->count(),
            reinterpret_cast<const GLfloat*>(uniform->data()));
        break;
    case GL_FLOAT_VEC3:
        glUniform3fv(
            location,
            uniform->count(),
            reinterpret_cast<const GLfloat*>(uniform->data()));
        break;
    case GL_FLOAT_VEC4:
        glUniform4fv(
            location,
            uniform->count(),
            reinterpret_cast<const GLfloat*>(uniform->data()));
        break;
    case GL_FLOAT_MAT2:
        glUniformMatrix2fv(
            location,
            uniform->count(),
            GL_FALSE,
            reinterpret_cast<const GLfloat*>(uniform->data()));
        break;
    case GL_FLOAT_MAT3:
        glUniformMatrix3fv(
            location,
            uniform->count(),
            GL_FALSE,
            reinterpret_cast<const GLfloat*>(uniform->data()));
        break;
    case GL_FLOAT_MAT4:
        glUniformMatrix4fv(
            location,
            uniform->count(),
            GL_FALSE,
            reinterpret_cast<const GLfloat*>(uniform->data()));
        break;
    }
}
