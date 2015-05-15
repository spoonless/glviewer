#include "gl.hpp"

std::size_t ogl::glSizeof(GLenum glType)
{
    switch(glType)
    {
    case GL_FLOAT:
        return sizeof(GLfloat);
    case GL_FLOAT_VEC2:
        return sizeof(GLfloat) * 2;
    case GL_FLOAT_VEC3:
        return sizeof(GLfloat) * 3;
    case GL_FLOAT_VEC4:
        return sizeof(GLfloat) * 4;
    case GL_FLOAT_MAT2:
        return sizeof(GLfloat) * 4;
    case GL_FLOAT_MAT3:
        return sizeof(GLfloat) * 9;
    case GL_FLOAT_MAT4:
        return sizeof(GLfloat) * 16;
    case GL_FLOAT_MAT2x3:
        return sizeof(GLfloat) * 6;
    case GL_FLOAT_MAT2x4:
        return sizeof(GLfloat) * 8;
    case GL_FLOAT_MAT3x2:
        return sizeof(GLfloat) * 6;
    case GL_FLOAT_MAT3x4:
        return sizeof(GLfloat) * 12;
    case GL_FLOAT_MAT4x2:
        return sizeof(GLfloat) * 8;
    case GL_FLOAT_MAT4x3:
        return sizeof(GLfloat) * 12;
    case GL_INT:
        return sizeof(GLint);
    case GL_INT_VEC2:
        return sizeof(GLint)* 2;
    case GL_INT_VEC3:
        return sizeof(GLint) * 3;
    case GL_INT_VEC4:
        return sizeof(GLint) * 4;
    case GL_UNSIGNED_INT:
        return sizeof(GLint);
    case GL_UNSIGNED_INT_VEC2:
        return sizeof(GLint) * 2;
    case GL_UNSIGNED_INT_VEC3:
        return sizeof(GLint) * 3;
    case GL_UNSIGNED_INT_VEC4:
        return sizeof(GLint) * 4;
    case GL_DOUBLE:
        return sizeof(GLdouble);
    case GL_DOUBLE_VEC2:
        return sizeof(GLdouble) * 2;
    case GL_DOUBLE_VEC3:
        return sizeof(GLdouble) * 3;
    case GL_DOUBLE_VEC4:
        return sizeof(GLdouble) * 4;
    case GL_DOUBLE_MAT2:
        return sizeof(GLdouble) * 4;
    case GL_DOUBLE_MAT3:
        return sizeof(GLdouble) * 9;
    case GL_DOUBLE_MAT4:
        return sizeof(GLdouble) * 16;
    case GL_DOUBLE_MAT2x3:
        return sizeof(GLdouble) * 6;
    case GL_DOUBLE_MAT2x4:
        return sizeof(GLdouble) * 8;
    case GL_DOUBLE_MAT3x2:
        return sizeof(GLdouble) * 6;
    case GL_DOUBLE_MAT3x4:
        return sizeof(GLdouble) * 12;
    case GL_DOUBLE_MAT4x2:
        return sizeof(GLdouble) * 8;
    case GL_DOUBLE_MAT4x3:
        return sizeof(GLdouble) * 12;
    case GL_BOOL:
        return sizeof(GLboolean);
    case GL_BOOL_VEC2:
        return sizeof(GLboolean) * 2;
    case GL_BOOL_VEC3:
        return sizeof(GLboolean) * 3;
    case GL_BOOL_VEC4:
        return sizeof(GLboolean) * 4;
    case GL_BYTE:
        return sizeof(GLbyte);
    case GL_UNSIGNED_BYTE:
        return sizeof(GLubyte);
    case GL_SHORT:
        return sizeof(GLshort);
    case GL_UNSIGNED_SHORT:
        return sizeof(GLushort);
    }
    return 0;
}
