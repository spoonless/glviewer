#include <gtest/gtest.h>
#include <UniformDeclaration.hpp>

using namespace glv;

TEST(UniformDeclaration, canCopyUniformDeclaration)
{
    UniformDeclaration uniformDeclaration1(1, 10, GL_FLOAT_MAT2, "uniformDeclaration1");
    UniformDeclaration uniformDeclaration2(2, 20, GL_FLOAT_MAT3, "uniformDeclaration2");

    ASSERT_NE(uniformDeclaration1, uniformDeclaration2);

    uniformDeclaration1 = uniformDeclaration2;
    ASSERT_EQ(uniformDeclaration1, uniformDeclaration2);
}

TEST(UniformDeclaration, canNormalizeArrayUniformDeclaration)
{
    UniformDeclaration uniformDeclaration(1, 10, GL_FLOAT_MAT2, "array[0]");
    ASSERT_EQ("array", uniformDeclaration.getName());

    uniformDeclaration = UniformDeclaration(1, 10, GL_FLOAT_MAT2, "anotherArray");
    ASSERT_EQ("anotherArray", uniformDeclaration.getName());
}
