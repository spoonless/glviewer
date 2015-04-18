#ifndef  GL_HPP
#define GL_HPP

#define GLSL_VERSION_HEADER "#version 330\n"

#include <cstdlib>
#include "glad/glad.h"

namespace gl
{
    std::size_t glSizeof(GLenum glType);
}

#endif // GL_HPP
