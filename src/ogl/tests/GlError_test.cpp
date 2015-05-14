#include <gtest/gtest.h>
#include "GlError.hpp"

using namespace gl;

TEST(GlError, cannotDetectErrorHappenedBeforeInit)
{
    // force openGL error
    glCompileShader(1000);

    GlError glError;

    ASSERT_FALSE(glError.hasOccured());
    ASSERT_EQ((GLenum)GL_NO_ERROR, glError.getErrorFlag());
    ASSERT_EQ("cannotDetectErrorAtInit: No error detected.", glError.toString("cannotDetectErrorAtInit"));
}

TEST(GlError, canDetectErrorHappenedAfterInit)
{
    GlError glError;

    // force openGL error
    glCompileShader(1000);

    ASSERT_TRUE(glError.hasOccured());
    ASSERT_EQ((GLenum)GL_INVALID_VALUE, glError.getErrorFlag());
    std::string message = glError.toString("canDetectErrorAfterInit");
    ASSERT_EQ("canDetectErrorAfterInit: A numeric argument is out of range.", message);
}
