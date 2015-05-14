#include <gtest/gtest.h>
#include "gl.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/matrix.hpp"

using namespace gl;

TEST(GlError, canGetSizeOfGlType)
{
    ASSERT_EQ(sizeof(GLfloat), glSizeof(GL_FLOAT));
    ASSERT_EQ(sizeof(glm::vec2), glSizeof(GL_FLOAT_VEC2));
    ASSERT_EQ(sizeof(glm::vec3), glSizeof(GL_FLOAT_VEC3));
    ASSERT_EQ(sizeof(glm::vec4), glSizeof(GL_FLOAT_VEC4));
    ASSERT_EQ(sizeof(glm::mat2), glSizeof(GL_FLOAT_MAT2));
    ASSERT_EQ(sizeof(glm::mat3), glSizeof(GL_FLOAT_MAT3));
    ASSERT_EQ(sizeof(glm::mat4), glSizeof(GL_FLOAT_MAT4));
    ASSERT_EQ(sizeof(glm::mat2x3), glSizeof(GL_FLOAT_MAT2x3));
    ASSERT_EQ(sizeof(glm::mat2x4), glSizeof(GL_FLOAT_MAT2x4));
    ASSERT_EQ(sizeof(glm::mat3x2), glSizeof(GL_FLOAT_MAT3x2));
    ASSERT_EQ(sizeof(glm::mat3x4), glSizeof(GL_FLOAT_MAT3x4));
    ASSERT_EQ(sizeof(glm::mat4x2), glSizeof(GL_FLOAT_MAT4x2));
    ASSERT_EQ(sizeof(glm::mat4x3), glSizeof(GL_FLOAT_MAT4x3));

    ASSERT_EQ(sizeof(GLint), glSizeof(GL_INT));
    ASSERT_EQ(sizeof(glm::ivec2), glSizeof(GL_INT_VEC2));
    ASSERT_EQ(sizeof(glm::ivec3), glSizeof(GL_INT_VEC3));
    ASSERT_EQ(sizeof(glm::ivec4), glSizeof(GL_INT_VEC4));

    ASSERT_EQ(sizeof(GLuint), glSizeof(GL_UNSIGNED_INT));
    ASSERT_EQ(sizeof(glm::uvec2), glSizeof(GL_UNSIGNED_INT_VEC2));
    ASSERT_EQ(sizeof(glm::uvec3), glSizeof(GL_UNSIGNED_INT_VEC3));
    ASSERT_EQ(sizeof(glm::uvec4), glSizeof(GL_UNSIGNED_INT_VEC4));

    ASSERT_EQ(sizeof(GLboolean), glSizeof(GL_BOOL));
    ASSERT_EQ(sizeof(glm::bvec2), glSizeof(GL_BOOL_VEC2));
    ASSERT_EQ(sizeof(glm::bvec3), glSizeof(GL_BOOL_VEC3));
    ASSERT_EQ(sizeof(glm::bvec4), glSizeof(GL_BOOL_VEC4));

    ASSERT_EQ(sizeof(GLdouble), glSizeof(GL_DOUBLE));
    ASSERT_EQ(sizeof(glm::dvec2), glSizeof(GL_DOUBLE_VEC2));
    ASSERT_EQ(sizeof(glm::dvec3), glSizeof(GL_DOUBLE_VEC3));
    ASSERT_EQ(sizeof(glm::dvec4), glSizeof(GL_DOUBLE_VEC4));
    ASSERT_EQ(sizeof(glm::dmat2), glSizeof(GL_DOUBLE_MAT2));
    ASSERT_EQ(sizeof(glm::dmat3), glSizeof(GL_DOUBLE_MAT3));
    ASSERT_EQ(sizeof(glm::dmat4), glSizeof(GL_DOUBLE_MAT4));
    ASSERT_EQ(sizeof(glm::dmat2x3), glSizeof(GL_DOUBLE_MAT2x3));
    ASSERT_EQ(sizeof(glm::dmat2x4), glSizeof(GL_DOUBLE_MAT2x4));
    ASSERT_EQ(sizeof(glm::dmat3x2), glSizeof(GL_DOUBLE_MAT3x2));
    ASSERT_EQ(sizeof(glm::dmat3x4), glSizeof(GL_DOUBLE_MAT3x4));
    ASSERT_EQ(sizeof(glm::dmat4x2), glSizeof(GL_DOUBLE_MAT4x2));
    ASSERT_EQ(sizeof(glm::dmat4x3), glSizeof(GL_DOUBLE_MAT4x3));

    ASSERT_EQ(sizeof(GLbyte), glSizeof(GL_BYTE));
    ASSERT_EQ(sizeof(GLubyte), glSizeof(GL_UNSIGNED_BYTE));

    ASSERT_EQ(sizeof(GLshort), glSizeof(GL_SHORT));
    ASSERT_EQ(sizeof(GLushort), glSizeof(GL_UNSIGNED_SHORT));

    ASSERT_EQ(static_cast<std::size_t>(0), glSizeof(GL_SAMPLER));
}
