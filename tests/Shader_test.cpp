#include <utility>
#include <gtest/gtest.h>
#include "Shader.hpp"

using namespace glv;

namespace
{

GLint getShaderType(const Shader &shader)
{
    GLint shaderType = 0;
    glGetShaderiv(shader.getId(), GL_SHADER_TYPE, &shaderType);
    return shaderType;
}

}

TEST(Shader, canCreateVertexShader)
{
    Shader shader(ShaderType::VERTEX_SHADER);

    ASSERT_NE(0u, shader.getId());
    ASSERT_TRUE(shader.exists());
    ASSERT_EQ(ShaderType::VERTEX_SHADER, shader.getType());
    ASSERT_EQ(GL_VERTEX_SHADER, getShaderType(shader));
}

TEST(Shader, canCreateFragmentShader)
{
    Shader shader(ShaderType::FRAGMENT_SHADER);

    ASSERT_NE(0u, shader.getId());
    ASSERT_TRUE(shader.exists());
    ASSERT_EQ(ShaderType::FRAGMENT_SHADER, shader.getType());
    ASSERT_EQ(GL_FRAGMENT_SHADER, getShaderType(shader));
}

TEST(Shader, canCompileShader)
{
    Shader shader(ShaderType::FRAGMENT_SHADER);
    ShaderCompilation compilationResult = shader.compile(GLSL_VERSION_HEADER
                                            "void main(){}");

    ASSERT_TRUE(compilationResult);
}

TEST(Shader, cannotCompileErroneousCode)
{
    Shader shader(ShaderType::FRAGMENT_SHADER);
    ShaderCompilation compilationResult = shader.compile("ERRONEOUS CODE");

    ASSERT_FALSE(compilationResult);
    ASSERT_NE("", compilationResult.message());
}

TEST(Shader, canGetSource)
{
    std::string source = "void main(){}";
    Shader shader(ShaderType::VERTEX_SHADER);
    shader.compile(source);

    std::string outputSource = shader.getSource();

    ASSERT_EQ(source, outputSource);
}

TEST(Shader, canMoveShader)
{
    Shader shader(ShaderType::FRAGMENT_SHADER);
    shader.compile("void main(){}");

    Shader movedShader = std::move(shader);

    ASSERT_FALSE(shader.exists());
    ASSERT_TRUE(movedShader.exists());
    ASSERT_EQ(ShaderType::FRAGMENT_SHADER, movedShader.getType());
    ASSERT_EQ("void main(){}", movedShader.getSource());
}
