#include <utility>
#include <gtest/gtest.h>
#include "Shader.hpp"

using namespace glv;

namespace
{

GLint getShaderType(Shader shader)
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
    CompilationResult compilationResult = shader.compile(GLSL_VERSION_HEADER
                                            "void main(){}");

    ASSERT_TRUE(compilationResult);
}

TEST(Shader, cannotCompileErroneousCode)
{
    Shader shader(ShaderType::FRAGMENT_SHADER);
    CompilationResult compilationResult = shader.compile("ERRONEOUS CODE");

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

TEST(Shader, canCopyShader)
{
    Shader shader(ShaderType::FRAGMENT_SHADER);
    shader.compile("void main(){}");

    Shader copyShader = shader;

    ASSERT_NE(shader.getId(), copyShader.getId());
    ASSERT_TRUE(copyShader.exists());
    ASSERT_EQ(ShaderType::FRAGMENT_SHADER, copyShader.getType());
    ASSERT_EQ("void main(){}", copyShader.getSource());
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

TEST(Shader, canAssignShader)
{
    Shader shader(ShaderType::VERTEX_SHADER);
    shader.compile("void vertex(){}");

    Shader otherShader(ShaderType::VERTEX_SHADER);
    otherShader.compile("void otherVertex(){}");

    shader = otherShader;
    std::string source = shader.getSource();

    ASSERT_EQ(ShaderType::VERTEX_SHADER, shader.getType());
    ASSERT_EQ("void otherVertex(){}", source);
    ASSERT_EQ(GL_VERTEX_SHADER, getShaderType(shader));
}

TEST(Shader, cannotAssignShaderOfDifferentType)
{
    Shader shader(ShaderType::VERTEX_SHADER);
    shader.compile("void vertex(){}");

    Shader otherShader(ShaderType::FRAGMENT_SHADER);
    otherShader.compile("void fragment(){}");

    shader = otherShader;
    std::string source = shader.getSource();

    ASSERT_EQ(ShaderType::VERTEX_SHADER, shader.getType());
    ASSERT_EQ("void vertex(){}", source);
    ASSERT_EQ(GL_VERTEX_SHADER, getShaderType(shader));
}
