#include "gtest/gtest.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat2x2.hpp"
#include "glm/mat2x3.hpp"
#include "glm/mat2x4.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat3x2.hpp"
#include "glm/mat3x4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/mat4x2.hpp"
#include "glm/mat4x3.hpp"
#include "ShaderProgram.hpp"

using namespace glv;

namespace
{

const char VALID_VERTEX_SHADER_SOURCE [] = GLSL_VERSION_HEADER
                                           "void main(){gl_Position = vec4(0);}";

const char VALID_FRAGMENT_SHADER_SOURCE [] = GLSL_VERSION_HEADER
                                             "void main(){}";

void addShader(ShaderProgram& shaderProgram, ShaderType type, const char* source)
{
    Shader vertexShader(type);
    EXPECT_TRUE(vertexShader.compile(source));

    ASSERT_TRUE(shaderProgram.attach(vertexShader));
}

template<typename T> void checkUniform(ShaderProgram &shaderProgram, const char *uniformName, const T &expectedValue)
{
    UniformDeclaration u = shaderProgram.getActiveUniform(uniformName);

    T v = *u;
    ASSERT_EQ(expectedValue, v);

    T modifiedValue = v + v;
    *u = modifiedValue;
    v = *u;
    ASSERT_EQ(modifiedValue, v);
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
    Shader shader(ShaderType::FRAGMENT_SHADER);

    ASSERT_TRUE(shaderProgram.attach(shader));

    ASSERT_TRUE(shaderProgram.has(shader));

    ASSERT_TRUE(shaderProgram.detach(shader));
    ASSERT_FALSE(shaderProgram.has(shader));
}


TEST(ShaderProgram, canDetachAllShaders)
{
    ShaderProgram shaderProgram;
    Shader shader1(ShaderType::VERTEX_SHADER);
    Shader shader2(ShaderType::FRAGMENT_SHADER);

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

    LinkResult lr = shaderProgram.link();

    ASSERT_FALSE(lr);
    ASSERT_EQ("Cannot link program because no shader is attached!", lr.message());
}

TEST(ShaderProgram, cannotLinkProgramWhenShaderCannotCompile)
{
    ShaderProgram shaderProgram;
    Shader shader(ShaderType::FRAGMENT_SHADER);
    shader.compile("invalid shader");

    shaderProgram.attach(shader);

    LinkResult lr = shaderProgram.link();

    ASSERT_FALSE(lr);
    ASSERT_FALSE(lr.message().empty());
}

TEST(ShaderProgram, canLinkProgramWhenShadersCompiled)
{
    ShaderProgram shaderProgram;

    Shader fragmentShader(ShaderType::FRAGMENT_SHADER);
    shaderProgram.attach(fragmentShader);
    fragmentShader.compile(VALID_FRAGMENT_SHADER_SOURCE);

    ASSERT_TRUE(shaderProgram.link());

    Shader vertexShader(ShaderType::VERTEX_SHADER);
    vertexShader.compile(VALID_VERTEX_SHADER_SOURCE);
    shaderProgram.attach(vertexShader);

    ASSERT_TRUE(shaderProgram.link());
}

TEST(ShaderProgram, canCopyProgram)
{
    ShaderProgram shaderProgram;

    Shader fragmentShader(ShaderType::FRAGMENT_SHADER);
    fragmentShader.compile(VALID_FRAGMENT_SHADER_SOURCE);
    shaderProgram.attach(fragmentShader);

    Shader vertexShader(ShaderType::VERTEX_SHADER);
    vertexShader.compile(VALID_VERTEX_SHADER_SOURCE);
    shaderProgram.attach(vertexShader);

    ShaderProgram copy = shaderProgram;

    ASSERT_TRUE(copy.exists());
    ASSERT_NE(0u, copy.getId());
    ASSERT_TRUE(glIsProgram(copy.getId()) == GL_TRUE);
    ASSERT_TRUE(copy.has(fragmentShader));
    ASSERT_TRUE(copy.has(vertexShader));
    ASSERT_TRUE(copy.link());
}

TEST(ShaderProgram, canAssignProgram)
{
    ShaderProgram shaderProgram;

    Shader fragmentShader(ShaderType::FRAGMENT_SHADER);
    fragmentShader.compile(VALID_FRAGMENT_SHADER_SOURCE);
    shaderProgram.attach(fragmentShader);

    Shader vertexShader(ShaderType::VERTEX_SHADER);
    vertexShader.compile(VALID_VERTEX_SHADER_SOURCE);
    shaderProgram.attach(vertexShader);

    ShaderProgram copy;
    Shader anotherShader(ShaderType::VERTEX_SHADER);
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

    ValidationResult vr = shaderProgram.validate();

    ASSERT_FALSE(vr);
    ASSERT_FALSE(vr.message().empty());
}

TEST(ShaderProgram, canValidateLinkedProgramWhenShadersCompiled)
{
    ShaderProgram shaderProgram;

    addShader(shaderProgram, ShaderType::FRAGMENT_SHADER, VALID_FRAGMENT_SHADER_SOURCE);
    ASSERT_TRUE(shaderProgram.link());

    addShader(shaderProgram, ShaderType::VERTEX_SHADER, VALID_VERTEX_SHADER_SOURCE);
    ASSERT_TRUE(shaderProgram.link());

    shaderProgram.link();

    ASSERT_TRUE(shaderProgram.validate());
}

TEST(ShaderProgram, canExtractEmptyUniformDeclarationWhenNoUniform)
{
    ShaderProgram shaderProgram;
    addShader(shaderProgram, ShaderType::FRAGMENT_SHADER, VALID_FRAGMENT_SHADER_SOURCE);
    ASSERT_TRUE(shaderProgram.link());

    UniformDeclarationVector uniformDeclarationVector;
    shaderProgram.extractActive(uniformDeclarationVector);

    ASSERT_TRUE(uniformDeclarationVector.empty());
}

TEST(ShaderProgram, canExtractUniformDeclarationWhenOneUniform)
{
    ShaderProgram shaderProgram;
    const char* source =
            GLSL_VERSION_HEADER
            "uniform vec4 position;"
            "void main() {"
            " gl_Position = position;"
            "}";

    addShader(shaderProgram, ShaderType::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());

    UniformDeclarationVector uniformDeclarationVector;
    shaderProgram.extractActive(uniformDeclarationVector);

    ASSERT_EQ(static_cast<size_t>(1), uniformDeclarationVector.size());
    ASSERT_EQ(UniformDeclaration(shaderProgram.getId(), 0, 1, GL_FLOAT_VEC4, "position"), uniformDeclarationVector[0]);
}

TEST(ShaderProgram, canGetActiveUniformDeclaration)
{
    ShaderProgram shaderProgram;
    const char* source =
            GLSL_VERSION_HEADER
            "uniform vec4 position;"
            "void main() {"
            " gl_Position = position;"
            "}";

    addShader(shaderProgram, ShaderType::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());

    UniformDeclaration result = shaderProgram.getActiveUniform("position");

    ASSERT_TRUE(result);
    ASSERT_EQ(UniformDeclaration(shaderProgram.getId(), 0, 1, GL_FLOAT_VEC4, "position"), result);
}

TEST(ShaderProgram, canExtractFixedArrayUniformDeclaration)
{
    ShaderProgram shaderProgram;
    const char* source =
            GLSL_VERSION_HEADER
            "uniform vec4 position[2];"
            "void main() {"
            " gl_Position = position[0] + position[1];"
            "}";

    addShader(shaderProgram, ShaderType::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());

    UniformDeclarationVector uniformDeclarationVector;
    shaderProgram.extractActive(uniformDeclarationVector);

    ASSERT_EQ(static_cast<size_t>(1), uniformDeclarationVector.size());
    ASSERT_EQ(UniformDeclaration(shaderProgram.getId(), 0, 2, GL_FLOAT_VEC4, "position[0]"), uniformDeclarationVector[0]);
}

TEST(ShaderProgram, canExtractArrayUniformDeclaration)
{
    ShaderProgram shaderProgram;
    const char* source =
            GLSL_VERSION_HEADER
            "uniform vec4 position[];"
            "void main() {"
            " gl_Position = position[0] + position[1] + position[3];"
            "}";

    addShader(shaderProgram, ShaderType::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());

    UniformDeclarationVector uniformDeclarationVector;
    shaderProgram.extractActive(uniformDeclarationVector);

    ASSERT_EQ(static_cast<size_t>(1), uniformDeclarationVector.size());
    ASSERT_EQ(UniformDeclaration(shaderProgram.getId(), 0, 4, GL_FLOAT_VEC4, "position[0]"), uniformDeclarationVector[0]);
}

TEST(ShaderProgram, canExtractStructUniformDeclaration)
{
    ShaderProgram shaderProgram;
    const char* source =
            GLSL_VERSION_HEADER
            "struct MyStruct {"
            " vec4 position1;"
            " vec3 position2;"
            "};"
            "uniform MyStruct ms;"
            "void main() {"
            " gl_Position = ms.position1 + vec4(ms.position2, 0);"
            "}";

    addShader(shaderProgram, ShaderType::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());

    UniformDeclarationVector uniformDeclarationVector;
    shaderProgram.extractActive(uniformDeclarationVector);

    ASSERT_EQ(static_cast<size_t>(2), uniformDeclarationVector.size());
    ASSERT_EQ(UniformDeclaration(shaderProgram.getId(), 0, 1, GL_FLOAT_VEC4, "ms.position1"), uniformDeclarationVector[0]);
    ASSERT_EQ(UniformDeclaration(shaderProgram.getId(), 1, 1, GL_FLOAT_VEC3, "ms.position2"), uniformDeclarationVector[1]);
}

TEST(ShaderProgram, canExtractStructArrayUniformDeclaration)
{
    ShaderProgram shaderProgram;
    const char* source =
            GLSL_VERSION_HEADER
            "struct MyStruct {"
            " vec4 position1[2];"
            " vec3 position2;"
            "};"
            "uniform MyStruct ms[2];"
            "void main() {"
            " gl_Position = ms[1].position1[1] + vec4(ms[1].position2, 0);"
            "}";

    addShader(shaderProgram, ShaderType::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());

    UniformDeclarationVector uniformDeclarationVector;
    shaderProgram.extractActive(uniformDeclarationVector);

    ASSERT_EQ(static_cast<size_t>(2), uniformDeclarationVector.size());
    ASSERT_EQ(UniformDeclaration(shaderProgram.getId(), 0, 2, GL_FLOAT_VEC4, "ms[1].position1[0]"), uniformDeclarationVector[0]);
    ASSERT_EQ(UniformDeclaration(shaderProgram.getId(), 2, 1, GL_FLOAT_VEC3, "ms[1].position2"), uniformDeclarationVector[1]);
}

TEST(ShaderProgram, canExtractMultipleUniformDeclaration)
{
    ShaderProgram shaderProgram;
    const char* source =
            GLSL_VERSION_HEADER
            "uniform vec4 position;"
            "uniform mat4 mvp;"
            "void main() {"
            " gl_Position = mvp * position;"
            "}";

    addShader(shaderProgram, ShaderType::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());

    UniformDeclarationVector uniformDeclarationVector;
    shaderProgram.extractActive(uniformDeclarationVector);

    ASSERT_EQ(static_cast<size_t>(2), uniformDeclarationVector.size());

    for (unsigned int i = 0; i < uniformDeclarationVector.size(); ++i)
    {
        UniformDeclaration& uniformDeclaration = uniformDeclarationVector[i];
        ASSERT_EQ(static_cast<GLuint>(i), uniformDeclaration.index());
        if (uniformDeclaration.name() == "position")
        {
            ASSERT_EQ(1, uniformDeclaration.size());
            ASSERT_EQ(static_cast<GLenum>(GL_FLOAT_VEC4), uniformDeclaration.type());
        }
        else if (uniformDeclaration.name() == "mvp")
        {
            ASSERT_EQ(1, uniformDeclaration.size());
            ASSERT_EQ(static_cast<GLenum>(GL_FLOAT_MAT4), uniformDeclaration.type());
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
    addShader(shaderProgram, ShaderType::FRAGMENT_SHADER, VALID_FRAGMENT_SHADER_SOURCE);
    ASSERT_TRUE(shaderProgram.link());

    VertexAttributeDeclarationVector vector;
    shaderProgram.extractActive(vector);

    ASSERT_TRUE(vector.empty());
}

TEST(ShaderProgram, canExtractAttributeDeclarationWhenOneAttribute)
{
    ShaderProgram shaderProgram;
    const char* source =
            GLSL_VERSION_HEADER
            "in vec4 vertices;"
            "void main() {"
            " gl_Position = vertices;"
            "}";

    addShader(shaderProgram, ShaderType::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());

    VertexAttributeDeclarationVector vector;
    shaderProgram.extractActive(vector);

    ASSERT_EQ(static_cast<size_t>(1), vector.size());
    ASSERT_EQ(VertexAttributeDeclaration(0, 1, GL_FLOAT_VEC4, "vertices"), vector[0]);
}

TEST(ShaderProgram, canExtractAttributeDeclarationWhenSeveralAttributes)
{
    ShaderProgram shaderProgram;
    const char* source =
            GLSL_VERSION_HEADER
            "in float magnitude;"
            "layout(location=2) in vec4 vertices;"
            "void main() {"
            " gl_Position = vertices * magnitude;"
            "}";

    addShader(shaderProgram, ShaderType::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());

    VertexAttributeDeclarationVector vector;
    shaderProgram.extractActive(vector);

    ASSERT_EQ(static_cast<size_t>(2), vector.size());
    ASSERT_EQ(VertexAttributeDeclaration(0, 1, GL_FLOAT, "magnitude"), vector[0]);
    ASSERT_EQ(VertexAttributeDeclaration(2, 1, GL_FLOAT_VEC4, "vertices"), vector[1]);
}

TEST(ShaderProgram, canExtractUniformFloatVectorValue)
{
    ShaderProgram shaderProgram;
    const char* source =
            GLSL_VERSION_HEADER
            "uniform float value = -12.45;"
            "uniform vec2 value2 = vec2(10.0,20.0);"
            "uniform vec3 value3 = vec3(10.0,20.0,30.0);"
            "uniform vec4 value4 = vec4(10.0,20.0,30.0,40.0);"
            "void main() {"
            " gl_Position = vec4(value, value2.x, value3.x, value4.x);"
            "}";

    addShader(shaderProgram, ShaderType::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());
    shaderProgram.use();

    checkUniform(shaderProgram, "value", -12.45f);
    checkUniform(shaderProgram, "value2", glm::fvec2(10.0f, 20.0f));
    checkUniform(shaderProgram, "value3", glm::fvec3(10.0f, 20.0f, 30.0f));
    checkUniform(shaderProgram, "value4", glm::fvec4(10.0f, 20.0f, 30.0f, 40.0f));
}

TEST(ShaderProgram, canExtractUniformBooleanVectorValue)
{
    ShaderProgram shaderProgram;
    const char* source =
            GLSL_VERSION_HEADER
            "uniform bool value = true;"
            "uniform bvec2 value2 = bvec2(true,false);"
            "uniform bvec3 value3 = bvec3(true, false, true);"
            "uniform bvec4 value4 = bvec4(true, false, true, false);"
            "void main() {"
            " if (value && value2.x && value3.x && value4.x)"
            " {"
            "   gl_Position = vec4(1.0);"
            " }"
            "}";

    addShader(shaderProgram, ShaderType::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());
    shaderProgram.use();

    checkUniform(shaderProgram, "value", true);
    checkUniform(shaderProgram, "value2", glm::bvec2(true, false));
    checkUniform(shaderProgram, "value3", glm::bvec3(true, false, true));
    checkUniform(shaderProgram, "value4", glm::bvec4(true, false, true, false));
}

TEST(ShaderProgram, canExtractUniformIntegerVectorValue)
{
    ShaderProgram shaderProgram;
    const char* source =
            GLSL_VERSION_HEADER
            "uniform int value = -1;"
            "uniform ivec2 value2 = ivec2(10,20);"
            "uniform ivec3 value3 = ivec3(10,20,30);"
            "uniform ivec4 value4 = ivec4(10,20,30,40);"
            "void main() {"
            " gl_Position = vec4(value, value2.x, value3.x, value4.x);"
            "}";

    addShader(shaderProgram, ShaderType::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());
    shaderProgram.use();

    checkUniform(shaderProgram, "value", -1);
    checkUniform(shaderProgram, "value2", glm::ivec2(10, 20));
    checkUniform(shaderProgram, "value3", glm::ivec3(10, 20, 30));
    checkUniform(shaderProgram, "value4", glm::ivec4(10, 20, 30, 40));
}

TEST(ShaderProgram, canExtractUniformUnsignedIntegerVectorValue)
{
    ShaderProgram shaderProgram;
    const char* source =
            GLSL_VERSION_HEADER
            "uniform uint value = 1u;"
            "uniform uvec2 value2 = uvec2(10u,20u);"
            "uniform uvec3 value3 = uvec3(10u,20u,30u);"
            "uniform uvec4 value4 = uvec4(10u,20u,30u,40u);"
            "void main() {"
            " gl_Position = vec4(value, value2.x, value3.x, value4.x);"
            "}";

    addShader(shaderProgram, ShaderType::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());
    shaderProgram.use();

    checkUniform(shaderProgram, "value", 1u);
    checkUniform(shaderProgram, "value2", glm::uvec2(10, 20));
    checkUniform(shaderProgram, "value3", glm::uvec3(10, 20, 30));
    checkUniform(shaderProgram, "value4", glm::uvec4(10, 20, 30, 40));
}

TEST(ShaderProgram, canExtractUniformMatrix2Value)
{
    ShaderProgram shaderProgram;
    const char* source =
            GLSL_VERSION_HEADER
            "uniform mat2 value = mat2x2(1,2,10,20);"
            "uniform mat2x3 value2 = mat2x3(1,2,3,10,20,30);"
            "uniform mat2x4 value3 = mat2x4(1,2,3,4,10,20,30,40);"
            "void main() {"
            " gl_Position = vec4(value[0][0], value2[0][0], value3[0][0], 0);"
            "}";

    addShader(shaderProgram, ShaderType::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());
    shaderProgram.use();

    checkUniform(shaderProgram, "value", glm::f32mat2x2(1,2,10,20));
    checkUniform(shaderProgram, "value2", glm::f32mat2x3(1,2,3,10,20,30));
    checkUniform(shaderProgram, "value3", glm::f32mat2x4(1,2,3,4,10,20,30,40));
}

TEST(ShaderProgram, canExtractUniformMatrix3Value)
{
    ShaderProgram shaderProgram;
    const char* source =
            GLSL_VERSION_HEADER
            "uniform mat3 value = mat3x3(1,2,3,10,20,30,100,200,300);"
            "uniform mat3x2 value2 = mat3x2(1,2,10,20,100,200);"
            "uniform mat3x4 value3 = mat3x4(1,2,3,4,10,20,30,40,100,200,300,400);"
            "void main() {"
            " gl_Position = vec4(value[0][0], value2[0][0], value3[0][0], 0);"
            "}";

    addShader(shaderProgram, ShaderType::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());
    shaderProgram.use();

    checkUniform(shaderProgram, "value", glm::f32mat3(1,2,3,10,20,30,100,200,300));
    checkUniform(shaderProgram, "value2", glm::f32mat3x2(1,2,10,20,100,200));
    checkUniform(shaderProgram, "value3", glm::f32mat3x4(1,2,3,4,10,20,30,40,100,200,300,400));
}

TEST(ShaderProgram, canExtractUniformMatrix4Value)
{
    ShaderProgram shaderProgram;
    const char* source =
            GLSL_VERSION_HEADER
            "uniform mat4 value = mat4x4(1,2,3,4,10,20,30,40,100,200,300,400,1000,2000,3000,4000);"
            "uniform mat4x2 value2 = mat4x2(1,2,10,20,100,200,1000,2000);"
            "uniform mat4x3 value3 = mat4x3(1,2,3,4,10,20,30,40,100,200,300,400);"
            "void main() {"
            " gl_Position = vec4(value[0][0], value2[0][0], value3[0][0], 0);"
            "}";

    addShader(shaderProgram, ShaderType::VERTEX_SHADER, source);
    ASSERT_TRUE(shaderProgram.link());
    shaderProgram.use();

    checkUniform(shaderProgram, "value", glm::f32mat4(1,2,3,4,10,20,30,40,100,200,300,400,1000,2000,3000,4000));
    checkUniform(shaderProgram, "value2", glm::f32mat4x2(1,2,10,20,100,200,1000,2000));
    checkUniform(shaderProgram, "value3", glm::f32mat4x3(1,2,3,4,10,20,30,40,100,200,300,400));
}
