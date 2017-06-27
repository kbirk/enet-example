#include "gl/Shader.h"

#include <fstream>
#include <regex>

Shader::Shared Shader::alloc()
{
    return std::make_shared<Shader>();
}

Shader::Shader()
    : id_(0)
{
}

Shader::~Shader()
{
    if (id_ != 0) {
        glDeleteProgram(id_);
        LOG_OPENGL("glDeleteProgram");
        id_ = 0;
    }
}

void Shader::use() const
{
    glUseProgram(id_);
}

bool Shader::isGLSL(const std::string& source) const
{
    static std::string REGEX_STR("[^]*void\\s+main\\s*\\(\\s*(void)*\\s*\\)\\s*[^]*");
    static std::regex GLSL_REGEX(REGEX_STR, std::regex::ECMAScript | std::regex::icase);
    return std::regex_match(source, GLSL_REGEX);
}

GLuint Shader::createShader(const std::string& input, GLenum type)
{
    std::string source;
    if (isGLSL(input)) {
        // source is already glsl
        source = input;
    } else {
        // load shader file into memory
        source = readShaderFile(input);
        // check for error
        if (source.empty()) {
            return 0;
        }
    }
    // create shader object
    GLuint shader = glCreateShader(type);
    LOG_OPENGL("glCreateShader");
    // set source code of shader object
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    LOG_OPENGL("glShaderSource");
    // return shader object
    return shader;
}

GLuint Shader::createShader(const std::vector<std::string>& inputs, GLenum type)
{
    std::vector<const char*> sources;
    for (auto& input : inputs) {
        std::string source;
        if (isGLSL(input)) {
            // source is already glsl
            source = input;
        } else {
            // load shader file into memory
            source = readShaderFile(input);
            // check for error
            if (source.empty()) {
                return 0;
            }
        }
        // add c-style string
        sources.push_back(source.c_str());
    }
    // create shader object
    GLuint shader = glCreateShader(type);
    LOG_OPENGL("glCreateShader");
    // set source code of shader object
    glShaderSource(shader, sources.size(), &sources[0], nullptr);
    LOG_OPENGL("glShaderSource");
    // return shader object
    return shader;
}

std::string Shader::readShaderFile(const std::string& filename) const
{
    LOG_INFO("Loading `" << filename << "` shader file");
    std::ifstream stream(filename);
    if (stream.is_open()) {
        // return source as a string
        return std::string(
            std::istreambuf_iterator<char>(stream),
            std::istreambuf_iterator<char>());
    }
    // print error message
    LOG_ERROR("Could not open shader file: " << filename);
    // return empty string
    return std::string();
}

bool Shader::compileShader(GLuint shader)
{
    // compile vertex shader
    glCompileShader(shader);
    LOG_OPENGL("glCompileShader");
    // error check
    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    LOG_OPENGL("glGetShaderiv");
    if (result == GL_FALSE) {
        // print error
        logCompileError(shader);
        // delete current objects and abort constructor
        glDeleteShader(shader);
        LOG_OPENGL("glDeleteShader");
        return false;
    }
    return true;
}

GLuint Shader::createAndCompileShader(const std::string& file, GLenum type)
{
    // create shader object
    GLuint shader = createShader(file, type);
    // compile shader object, check for error
    if (!compileShader(shader)) {
        return 0;
    }
    return shader;
}

GLuint Shader::createAndCompileShader(const std::vector<std::string>& files, GLenum type)
{
    // create shader object
    GLuint shader = createShader(files, type);
    // compile shader object, check for error
    if (!compileShader(shader)) {
        return 0;
    }
    return shader;
}

bool Shader::linkProgram()
{
    // link shader program
    glLinkProgram(id_);
    LOG_OPENGL("glLinkProgram");
    // error check
    GLint result;
    glGetProgramiv(id_, GL_LINK_STATUS, &result);
    LOG_OPENGL("glGetProgramiv");
    // check for error
    if (result == GL_FALSE) {
        // print error
        logLinkError();
        return false;
    }
    return true;
}

void Shader::logCompileError(GLuint shader) const
{
    // get info log length
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    LOG_OPENGL("glGetShaderiv");
    // get error message
    std::vector<char> msg(length);
    glGetShaderInfoLog(shader, length, nullptr, &msg[0]);
    LOG_OPENGL("glGetShaderInfoLog");
    // log error message
    LOG_ERROR("Compilation errors:\n"
        << std::string(msg.begin(), msg.end()));
}

void Shader::logLinkError() const
{
    // get info log length
    GLint length;
    glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &length);
    LOG_OPENGL("glGetProgramiv");
    // get error message
    std::vector<char> msg(length);
    glGetProgramInfoLog(id_, length, nullptr, &msg[0]);
    LOG_OPENGL("glGetProgramInfoLog");
    // print error message
    LOG_ERROR("Linking errors:\n"
        << std::string(msg.begin(), msg.end()));
}

void Shader::setUniform(const std::string& name, const Uniform::Shared& uniform, bool ignore)
{
    LOG_ERROR("Method not implemented");
}

void Shader::queryUniforms()
{
    // query all necessary uniform information
    std::vector<GLuint> uniformIndices = queryUniformIndices();
    std::vector<std::string> uniformNames = queryUniformNames(uniformIndices);
    std::vector<GLenum> uniformTypes = queryUniformTypes(uniformIndices);
    std::vector<GLint> parentBlockIndices = queryParentBlockIndices(uniformIndices);
    std::vector<GLint> uniformOffsets = queryUniformOffsets(uniformIndices);
    std::vector<GLint> uniformLocations = queryUniformLocations(uniformNames);

    // for each uniform index
    for (auto index : uniformIndices) {
        // check if part of a block or not
        if (parentBlockIndices[index] == -1) {
            // not in a block, add as uniform descriptor

            const auto& name = uniformNames[index];
            const auto& type = uniformTypes[index];
            const auto& location = uniformLocations[index];

            ////
            LOG_INFO("Uniform name " << name << " type " << type << " location " << location);
            ////

            descriptors_[name] = UniformDescriptor::alloc(name, type, location);
        }
    }

    // query all necessary uniform block information
    std::vector<GLuint> blockIndices = queryUniformBlockIndices();
    std::vector<std::string> blockNames = queryUniformBlockNames(blockIndices);
    std::vector<GLint> blockSizes = queryUniformBlockSizes(blockIndices);

    // for each block
    for (auto index : blockIndices) {

        // get all uniform offsets that are part of this block
        std::map<std::string, GLint> offsets;
        for (uint32_t i = 0; i < parentBlockIndices.size(); i++) {
            if (parentBlockIndices[i] == int32_t(index)) {
                // uniform is part of this block
                offsets[uniformNames[i]] = uniformOffsets[i];
            }
        }

        const auto& blockName = blockNames[index];
        const auto& blockIndex = blockIndices[index];
        const auto& blockSize = blockSizes[index];

        ////
        LOG_INFO("Uniform block name " << blockName << " index " << blockIndex << " size " << blockSize);
        for (auto& pair : offsets) {
            LOG_INFO("	" << pair.first << " offset " << pair.second);
        }
        ////

        // add block descriptor
        blockDescriptors_[blockName] = UniformBlockDescriptor::alloc(
            blockName,
            blockIndex,
            blockSize,
            offsets);

        // set binding point for block index and shader
        // TODO: make this configurable
        glUniformBlockBinding(id_, blockIndex, blockIndex);
        LOG_OPENGL("glUniformBlockBinding");
    }
}

std::vector<GLuint> Shader::queryUniformIndices() const
{
    // get the number of uniforms
    GLint numActiveUniforms = 0;
    glGetProgramiv(id_, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
    LOG_OPENGL("glGetProgramiv");
    // get uniform indices from 0 to GL_ACTIVE_UNIFORMS
    std::vector<GLuint> indices(numActiveUniforms);
    for (int i = 0; i < numActiveUniforms; i++) {
        indices[i] = i;
    }
    return indices;
}

std::vector<GLint> Shader::queryUniformLocations(const std::vector<std::string>& names) const
{
    std::vector<GLint> locations;
    for (auto& name : names) {
        locations.push_back(glGetUniformLocation(id_, name.c_str()));
        LOG_OPENGL("glGetActiveUniformsiv");
    }
    return locations;
}

std::vector<GLint> Shader::queryUniformOffsets(const std::vector<GLuint>& indices) const
{
    // get uniform offsets
    std::vector<GLint> offsets(indices.size());
    glGetActiveUniformsiv(id_, indices.size(), &indices[0], GL_UNIFORM_OFFSET, &offsets[0]);
    LOG_OPENGL("glGetActiveUniformsiv");
    return offsets;
}

std::vector<GLenum> Shader::queryUniformTypes(const std::vector<GLuint>& indices) const
{
    // get uniform types
    std::vector<GLint> types(indices.size());
    glGetActiveUniformsiv(id_, indices.size(), &indices[0], GL_UNIFORM_TYPE, &types[0]);
    LOG_OPENGL("glGetActiveUniformsiv");
    // cast to GLenum
    return std::vector<GLenum>(types.begin(), types.end());
    ;
}

std::vector<std::string> Shader::queryUniformNames(const std::vector<GLuint>& indices) const
{
    // get uniform name lengths
    std::vector<GLint> nameLengths(indices.size());
    glGetActiveUniformsiv(id_, indices.size(), &indices[0], GL_UNIFORM_NAME_LENGTH, &nameLengths[0]);
    LOG_OPENGL("glGetActiveUniformsiv");
    // for each uniform index
    std::vector<std::string> names;
    names.reserve(indices.size());
    for (auto index : indices) {
        // get uniform name
        GLint nameLength = nameLengths[index];
        std::vector<GLchar> nameData(nameLength);
        glGetActiveUniformName(id_, index, nameLength, nullptr, &nameData[0]);
        LOG_OPENGL("glGetActiveUniformName");
        // cast from char vector to string=
        names.push_back(std::string(nameData.begin(), nameData.end() - 1));
    }
    return names;
}

std::vector<GLint> Shader::queryParentBlockIndices(const std::vector<GLuint>& indices) const
{
    // get uniform block indices (-1 is not part of a block)
    std::vector<GLint> blockIndices(indices.size());
    glGetActiveUniformsiv(id_, indices.size(), &indices[0], GL_UNIFORM_BLOCK_INDEX, &blockIndices[0]);
    LOG_OPENGL("glGetActiveUniformsiv");
    return blockIndices;
}

std::vector<GLuint> Shader::queryUniformBlockIndices() const
{
    // get the number of active uniform blocks
    GLint numActiveBlocks = 0;
    glGetProgramiv(id_, GL_ACTIVE_UNIFORM_BLOCKS, &numActiveBlocks);
    LOG_OPENGL("glGetProgramiv");
    // get uniform indices from 0 to GL_ACTIVE_UNIFORMS
    std::vector<GLuint> indices;
    indices.reserve(numActiveBlocks);
    for (int i = 0; i < numActiveBlocks; i++) {
        indices.push_back(i);
    }
    return indices;
}

std::vector<GLint> Shader::queryUniformBlockSizes(const std::vector<GLuint>& indices) const
{
    std::vector<GLint> sizes;
    sizes.reserve(indices.size());
    for (auto index : indices) {
        GLint blockSize;
        glGetActiveUniformBlockiv(id_, index, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
        LOG_OPENGL("glGetActiveUniformBlockiv");
        sizes.push_back(blockSize);
    }
    return sizes;
}

std::vector<std::string> Shader::queryUniformBlockNames(const std::vector<GLuint>& indices) const
{
    std::vector<std::string> names;
    names.reserve(indices.size());
    for (auto index : indices) {
        // get the length of the name
        GLint nameLen = 0;
        glGetActiveUniformBlockiv(id_, index, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);
        LOG_OPENGL("glGetActiveUniformBlockiv");
        // get the block name
        std::vector<GLchar> nameData(nameLen);
        glGetActiveUniformBlockName(id_, index, nameLen, nullptr, &nameData[0]);
        LOG_OPENGL("glGetActiveUniformBlockName");
        // cast from char vector to string
        std::string name(nameData.begin(), nameData.end() - 1);
        // add to return vector
        names.push_back(name);
    }
    return names;
}
