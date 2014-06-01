#include <gtest/gtest.h>
#include <Shader.hpp>

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
    Shader shader(Shader::VERTEX_SHADER);

    ASSERT_NE(0u, shader.getId());
    ASSERT_TRUE(shader.exists());
    ASSERT_EQ(Shader::VERTEX_SHADER, shader.getType());
    ASSERT_EQ(GL_VERTEX_SHADER, getShaderType(shader));
}

TEST(Shader, canCreateFragmentShader)
{
    Shader shader(Shader::FRAGMENT_SHADER);

    ASSERT_NE(0u, shader.getId());
    ASSERT_TRUE(shader.exists());
    ASSERT_EQ(Shader::FRAGMENT_SHADER, shader.getType());
    ASSERT_EQ(GL_FRAGMENT_SHADER, getShaderType(shader));
}

TEST(Shader, canCompileShader)
{
    Shader shader(Shader::FRAGMENT_SHADER);
    bool compilationResult = shader.compile("void main(){}");

    ASSERT_TRUE(compilationResult);
}

TEST(Shader, cannotCompileErroneousCode)
{
    Shader shader(Shader::FRAGMENT_SHADER);
    bool compilationResult = shader.compile("ERRONEOUS CODE");

    ASSERT_FALSE(compilationResult);
    ASSERT_NE("", shader.getLastCompilationLog());
}

TEST(Shader, canExtractSource)
{
    std::string source = "void main(){}";
    Shader shader(Shader::VERTEX_SHADER);
    shader.compile(source);

    std::string outputSource;
    shader.extractSource(outputSource);

    ASSERT_EQ(source, outputSource);
}

TEST(Shader, canCopyShader)
{
    Shader shader(Shader::FRAGMENT_SHADER);
    shader.compile("void main(){}");

    Shader copyShader = shader;
    std::string copySource;
    copyShader.extractSource(copySource);

    ASSERT_NE(shader.getId(), copyShader.getId());
    ASSERT_TRUE(glIsShader(copyShader.getId()));
    ASSERT_EQ("void main(){}", copySource);
}

TEST(Shader, canAssignShader)
{
    Shader shader(Shader::VERTEX_SHADER);
    shader.compile("void vertex(){}");

    Shader otherShader(Shader::VERTEX_SHADER);
    otherShader.compile("void otherVertex(){}");

    shader = otherShader;
    std::string source;
    shader.extractSource(source);

    ASSERT_EQ(Shader::VERTEX_SHADER, shader.getType());
    ASSERT_EQ("void otherVertex(){}", source);
    ASSERT_EQ(GL_VERTEX_SHADER, getShaderType(shader));
}

TEST(Shader, cannotAssignShaderOfDifferentType)
{
    Shader shader(Shader::VERTEX_SHADER);
    shader.compile("void vertex(){}");

    Shader otherShader(Shader::FRAGMENT_SHADER);
    otherShader.compile("void fragment(){}");

    shader = otherShader;
    std::string source;
    shader.extractSource(source);

    ASSERT_EQ(Shader::VERTEX_SHADER, shader.getType());
    ASSERT_EQ("void vertex(){}", source);
    ASSERT_EQ(GL_VERTEX_SHADER, getShaderType(shader));
}
