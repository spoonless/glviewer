#include <gtest/gtest.h>
#include <UniformDeclaration.hpp>

using namespace ogl;

TEST(UniformDeclaration, canCopyUniformDeclaration)
{
    UniformDeclaration uniformDeclaration1(10u, 1, 10, GL_FLOAT_MAT2, "uniformDeclaration1");
    UniformDeclaration uniformDeclaration2(10u, 2, 20, GL_FLOAT_MAT3, "uniformDeclaration2");

    ASSERT_NE(uniformDeclaration1, uniformDeclaration2);

    uniformDeclaration1 = uniformDeclaration2;
    ASSERT_EQ(uniformDeclaration1, uniformDeclaration2);
}

TEST(UniformDeclaration, canNormalizeArrayUniformDeclaration)
{
    UniformDeclaration uniformDeclaration(10u, 1, 10, GL_FLOAT_MAT2, "array[0]");
    ASSERT_EQ("array", uniformDeclaration.name());

    uniformDeclaration = UniformDeclaration(10u, 1, 10, GL_FLOAT_MAT2, "anotherArray");
    ASSERT_EQ("anotherArray", uniformDeclaration.name());
}
