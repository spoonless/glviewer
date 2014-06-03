#include "gtest/gtest.h"
#include "ShaderProgram.hpp"

using namespace glv;

namespace
{

const char VALID_VERTEX_SHADER_SOURCE [] = "void main(){gl_Position = gl_Vertex;}";
const char VALID_FRAGMENT_SHADER_SOURCE [] = "void main(){}";

void addShader(ShaderProgram& shaderProgram, Shader::ShaderType type, const char* source)
{
    Shader vertexShader(type);
    ASSERT_TRUE(vertexShader.compile(source));
    ASSERT_TRUE(shaderProgram.attach(vertexShader));
}

}

TEST(ShaderProgram, canCreateShaderProgram)
{
    ShaderProgram shaderProgram;

    ASSERT_TRUE(shaderProgram.exists());
    ASSERT_NE(0u, shaderProgram.getId());
}

TEST(ShaderProgram, canAttachDetachShader)
{
    ShaderProgram shaderProgram;
    Shader shader(Shader::FRAGMENT_SHADER);

    ASSERT_TRUE(shaderProgram.attach(shader));

    ASSERT_TRUE(shaderProgram.has(shader));

    ASSERT_TRUE(shaderProgram.detach(shader));
    ASSERT_FALSE(shaderProgram.has(shader));
}


TEST(ShaderProgram, canDetachAllShaders)
{
    ShaderProgram shaderProgram;
    Shader shader1(Shader::VERTEX_SHADER);
    Shader shader2(Shader::FRAGMENT_SHADER);

    ASSERT_TRUE(shaderProgram.attach(shader1));
    ASSERT_TRUE(shaderProgram.attach(shader2));

    ASSERT_TRUE(shaderProgram.has(shader1));
    ASSERT_TRUE(shaderProgram.has(shader2));

    shaderProgram.detachAllShaders();

    ASSERT_FALSE(shaderProgram.has(shader1));
    ASSERT_FALSE(shaderProgram.has(shader2));
}


TEST(ShaderProgram, cannotLinkProgramWithoutShader)
{
    ShaderProgram shaderProgram;

    ASSERT_FALSE(shaderProgram.link());
    ASSERT_EQ("Cannot link program because no shader is attached!", shaderProgram.getLastLinkLog());
}

TEST(ShaderProgram, cannotLinkProgramWhenShaderNotCompiled)
{
    ShaderProgram shaderProgram;
    Shader shader(Shader::FRAGMENT_SHADER);

    shaderProgram.attach(shader);

    ASSERT_FALSE(shaderProgram.link());
    ASSERT_FALSE(shaderProgram.getLastLinkLog().empty());
}

TEST(ShaderProgram, canLinkProgramWhenShadersCompiled)
{
    ShaderProgram shaderProgram;

    Shader fragmentShader(Shader::FRAGMENT_SHADER);
    shaderProgram.attach(fragmentShader);
    fragmentShader.compile(VALID_FRAGMENT_SHADER_SOURCE);

    ASSERT_TRUE(shaderProgram.link());

    Shader vertexShader(Shader::VERTEX_SHADER);
    vertexShader.compile(VALID_VERTEX_SHADER_SOURCE);
    shaderProgram.attach(vertexShader);

    ASSERT_TRUE(shaderProgram.link());
}

TEST(ShaderProgram, canCopyProgram)
{
    ShaderProgram shaderProgram;

    Shader fragmentShader(Shader::FRAGMENT_SHADER);
    fragmentShader.compile(VALID_FRAGMENT_SHADER_SOURCE);
    shaderProgram.attach(fragmentShader);

    Shader vertexShader(Shader::VERTEX_SHADER);
    vertexShader.compile(VALID_VERTEX_SHADER_SOURCE);
    shaderProgram.attach(vertexShader);

    ShaderProgram copy = shaderProgram;

    ASSERT_TRUE(copy.exists());
    ASSERT_NE(0u, copy.getId());
    ASSERT_TRUE(glIsProgram(copy.getId()));
    ASSERT_TRUE(copy.has(fragmentShader));
    ASSERT_TRUE(copy.has(vertexShader));
    ASSERT_TRUE(copy.link());
}

TEST(ShaderProgram, canAssignProgram)
{
    ShaderProgram shaderProgram;

    Shader fragmentShader(Shader::FRAGMENT_SHADER);
    fragmentShader.compile(VALID_FRAGMENT_SHADER_SOURCE);
    shaderProgram.attach(fragmentShader);

    Shader vertexShader(Shader::VERTEX_SHADER);
    vertexShader.compile(VALID_VERTEX_SHADER_SOURCE);
    shaderProgram.attach(vertexShader);

    ShaderProgram copy;
    Shader anotherShader(Shader::VERTEX_SHADER);
    copy.attach(anotherShader);
    ASSERT_TRUE(copy.has(anotherShader));

    copy = shaderProgram;

    ASSERT_TRUE(copy.has(fragmentShader));
    ASSERT_TRUE(copy.has(vertexShader));
    ASSERT_FALSE(copy.has(anotherShader));
    ASSERT_TRUE(copy.link());
}

TEST(ShaderProgram, cannotValidateUnlinkProgram)
{
    ShaderProgram shaderProgram;

    ASSERT_FALSE(shaderProgram.validate());
    ASSERT_FALSE(shaderProgram.getLastValidationLog().empty());
}

TEST(ShaderProgram, canValidateLinkedProgramWhenShadersCompiled)
{
    ShaderProgram shaderProgram;

    addShader(shaderProgram, Shader::FRAGMENT_SHADER, VALID_FRAGMENT_SHADER_SOURCE);
    ASSERT_TRUE(shaderProgram.link());

    addShader(shaderProgram, Shader::VERTEX_SHADER, VALID_VERTEX_SHADER_SOURCE);
    ASSERT_TRUE(shaderProgram.link());

    shaderProgram.link();

    ASSERT_TRUE(shaderProgram.validate());
}

TEST(ShaderProgram, canExtractEmptyUniformDeclarationWhenNoUniform)
{
    ShaderProgram shaderProgram;
    addShader(shaderProgram, Shader::FRAGMENT_SHADER, VALID_FRAGMENT_SHADER_SOURCE);
    ASSERT_TRUE(shaderProgram.link());

    UniformDeclarationVector uniformDeclarationVector;
    shaderProgram.extractActive(uniformDeclarationVector);

    ASSERT_TRUE(uniformDeclarationVector.empty());
}

TEST(ShaderProgram, canExtractUniformDeclarationWhenOneUniform)
{
    ShaderProgram shaderProgram;
    const char* source = \
            "uniform vec4 position;"\
            "void main() {"\
            " gl_Position = position;"\
            "}";

    addShader(shaderProgram, Shader::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());

    UniformDeclarationVector uniformDeclarationVector;
    shaderProgram.extractActive(uniformDeclarationVector);

    ASSERT_EQ(static_cast<size_t>(1), uniformDeclarationVector.size());
    ASSERT_EQ(UniformDeclaration(0, 1, GL_FLOAT_VEC4, "position"), uniformDeclarationVector[0]);
}

TEST(ShaderProgram, canExtractFixedArrayUniformDeclaration)
{
    ShaderProgram shaderProgram;
    const char* source = \
            "uniform vec4 position[2];"\
            "void main() {"\
            " gl_Position = position[0] + position[1];"\
            "}";

    addShader(shaderProgram, Shader::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());

    UniformDeclarationVector uniformDeclarationVector;
    shaderProgram.extractActive(uniformDeclarationVector);

    ASSERT_EQ(static_cast<size_t>(1), uniformDeclarationVector.size());
    ASSERT_EQ(UniformDeclaration(0, 2, GL_FLOAT_VEC4, "position[0]"), uniformDeclarationVector[0]);
}

TEST(ShaderProgram, canExtractArrayUniformDeclaration)
{
    ShaderProgram shaderProgram;
    const char* source = \
            "uniform vec4 position[];"\
            "void main() {"\
            " gl_Position = position[0] + position[1] + position[3];"\
            "}";

    addShader(shaderProgram, Shader::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());

    UniformDeclarationVector uniformDeclarationVector;
    shaderProgram.extractActive(uniformDeclarationVector);

    ASSERT_EQ(static_cast<size_t>(1), uniformDeclarationVector.size());
    ASSERT_EQ(UniformDeclaration(0, 4, GL_FLOAT_VEC4, "position[0]"), uniformDeclarationVector[0]);
}

TEST(ShaderProgram, canExtractStructUniformDeclaration)
{
    ShaderProgram shaderProgram;
    const char* source = \
            "struct MyStruct {"\
            " vec4 position1;"\
            " vec3 position2;"\
            "};"\
            "uniform MyStruct ms;"\
            "void main() {"\
            " gl_Position = ms.position1 + vec4(ms.position2, 0);"\
            "}";

    addShader(shaderProgram, Shader::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());

    UniformDeclarationVector uniformDeclarationVector;
    shaderProgram.extractActive(uniformDeclarationVector);

    ASSERT_EQ(static_cast<size_t>(2), uniformDeclarationVector.size());
    ASSERT_EQ(UniformDeclaration(0, 1, GL_FLOAT_VEC4, "ms.position1"), uniformDeclarationVector[0]);
    ASSERT_EQ(UniformDeclaration(1, 1, GL_FLOAT_VEC3, "ms.position2"), uniformDeclarationVector[1]);
}

TEST(ShaderProgram, canExtractStructArrayUniformDeclaration)
{
    ShaderProgram shaderProgram;
    const char* source = \
            "struct MyStruct {"\
            " vec4 position1[2];"\
            " vec3 position2;"\
            "};"\
            "uniform MyStruct ms[2];"\
            "void main() {"\
            " gl_Position = ms[1].position1[1] + vec4(ms[1].position2, 0);"\
            "}";

    addShader(shaderProgram, Shader::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());

    UniformDeclarationVector uniformDeclarationVector;
    shaderProgram.extractActive(uniformDeclarationVector);

    ASSERT_EQ(static_cast<size_t>(2), uniformDeclarationVector.size());
    ASSERT_EQ(UniformDeclaration(0, 2, GL_FLOAT_VEC4, "ms[1].position1[0]"), uniformDeclarationVector[0]);
    ASSERT_EQ(UniformDeclaration(1, 1, GL_FLOAT_VEC3, "ms[1].position2"), uniformDeclarationVector[1]);
}

TEST(ShaderProgram, canExtractMultipleUniformDeclaration)
{
    ShaderProgram shaderProgram;
    const char* source = \
            "uniform vec4 position;"\
            "uniform mat4 mvp;"
            "void main() {"\
            " gl_Position = mvp * position;"\
            "}";

    addShader(shaderProgram, Shader::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());

    UniformDeclarationVector uniformDeclarationVector;
    shaderProgram.extractActive(uniformDeclarationVector);

    ASSERT_EQ(static_cast<size_t>(2), uniformDeclarationVector.size());

    for (unsigned int i = 0; i < uniformDeclarationVector.size(); ++i)
    {
        UniformDeclaration& uniformDeclaration = uniformDeclarationVector[i];
        ASSERT_EQ(static_cast<GLuint>(i), uniformDeclaration.getIndex());
        if (uniformDeclaration.getName() == "position")
        {
            ASSERT_EQ(1, uniformDeclaration.getSize());
            ASSERT_EQ(static_cast<GLenum>(GL_FLOAT_VEC4), uniformDeclaration.getType());
        }
        else if (uniformDeclaration.getName() == "mvp")
        {
            ASSERT_EQ(1, uniformDeclaration.getSize());
            ASSERT_EQ(static_cast<GLenum>(GL_FLOAT_MAT4), uniformDeclaration.getType());
        }
        else
        {
            ADD_FAILURE();
        }
    }
}

TEST(ShaderProgram, canExtractEmptyAttributeDeclarationWhenNoAttribute)
{
    ShaderProgram shaderProgram;
    addShader(shaderProgram, Shader::FRAGMENT_SHADER, VALID_FRAGMENT_SHADER_SOURCE);
    ASSERT_TRUE(shaderProgram.link());

    VertexAttributeDeclarationVector vector;
    shaderProgram.extractActive(vector);

    ASSERT_TRUE(vector.empty());
}

TEST(ShaderProgram, canExtractAttributeDeclarationWhenOneAttribute)
{
    ShaderProgram shaderProgram;
    const char* source = \
            "in vec4 vertices;"\
            "void main() {"\
            " gl_Position = vertices;"\
            "}";

    addShader(shaderProgram, Shader::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());

    VertexAttributeDeclarationVector vector;
    shaderProgram.extractActive(vector);

    ASSERT_EQ(static_cast<size_t>(1), vector.size());
    ASSERT_EQ(VertexAttributeDeclaration(0, 1, GL_FLOAT_VEC4, "vertices"), vector[0]);
}
