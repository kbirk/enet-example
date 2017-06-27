#include "gl/Uniform.h"

const GLubyte* Uniform::data() const
{
    return static_cast<const GLubyte*>(&data_[0]);
}

GLuint Uniform::numBytes() const
{
    return data_.size();
}

GLsizei Uniform::count() const
{
    return count_;
}
