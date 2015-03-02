#include <functional>
#include <cstring>
#include <memory>
#include <string>
#include "Duration.hpp"
#include "ShaderProgram.hpp"
#include "GlError.hpp"

using namespace glv;

namespace
{

std::string extractInfoLog(GLuint shaderProgramId)
{
    GlError error;
    GLint infoLogLength = 0;

    glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);

    if (infoLogLength == 0)
    {
        return std::string{};
    }

    auto infoLogBuffer = std::unique_ptr<GLchar[]>(new GLchar[infoLogLength]);
    glGetProgramInfoLog(shaderProgramId, infoLogLength, NULL, infoLogBuffer.get());
    if (error.hasOccured())
    {
        return error.toString("Cannot retrieve properly shader program link log info");
    }
    else
    {
        return std::string{infoLogBuffer.get()};
    }
}

inline GLuint getNbAttachedShaders(GLint shaderProgramId)
{
    GLint nbShaders = 0;
    glGetProgramiv(shaderProgramId, GL_ATTACHED_SHADERS, &nbShaders);
    return nbShaders;
}

void foreachAttachedShader(GLint shaderProgramId, std::function<bool(GLuint)> const &f)
{
    GLint nbShaders = getNbAttachedShaders(shaderProgramId);
    auto shaders = std::unique_ptr<GLuint[]>(new GLuint[nbShaders]);

    GLsizei count = 0;
    glGetAttachedShaders(shaderProgramId, nbShaders, &count, shaders.get());

    for (GLsizei i = 0; i < count; ++i)
    {
        if (!f(shaders[i]))
        {
            return;
        }
    }
}

}

ShaderProgram::ShaderProgram()
    : _shaderProgramId{glCreateProgram()}
{
}

ShaderProgram::ShaderProgram(ShaderProgram &&shaderProgram)
    : _shaderProgramId{shaderProgram._shaderProgramId}
{
    shaderProgram._shaderProgramId = 0;
}

ShaderProgram::~ShaderProgram()
{
    deleteShaderProgram();
}

bool ShaderProgram::exists() const
{
    return _shaderProgramId != 0 && glIsProgram(_shaderProgramId);
}

ShaderAttachmentResult ShaderProgram::attach(const Shader &shader)
{
    if (!shader.exists())
    {
        return ShaderAttachmentResult{false, "Attempt to attach a non shader object to GLSL program!"};
    }
    GlError error;
    glAttachShader(_shaderProgramId, shader.getId());
    return ShaderAttachmentResult{!error, error ? error.toString("Error while attempting to attach shader object to GLSL program") : ""};
}

bool ShaderProgram::has(const Shader& shader) const
{
    if (!shader.exists())
    {
        return false;
    }

    bool found = false;

    foreachAttachedShader(this->_shaderProgramId, [&found, &shader](GLuint shaderId) -> bool {
        found = shaderId == shader.getId();
        return !found;
    });

    return found;
}

ShaderAttachmentResult ShaderProgram::detach(const Shader& shader)
{
    if (!shader.exists())
    {
        return ShaderAttachmentResult{false, "Attempt to detach a non shader object to GLSL program!"};
    }
    GlError error;
    glDetachShader(_shaderProgramId, shader.getId());
    return ShaderAttachmentResult{!error, error ? error.toString("Error while attempting to detach shader object to GLSL program"): ""};
}

void ShaderProgram::detachAllShaders()
{
    foreachAttachedShader(this->_shaderProgramId, [this](GLuint shaderId) -> bool {
        glDetachShader(_shaderProgramId, shaderId);
        return true;
    });
}

LinkResult ShaderProgram::link()
{
    GlError error;

    /*
     * Checking programmatically that at least one shader is attached to this program.
     * NVidia and AMD cards have heterogeneous behaviors. Some allow link operation
     * for program with no attached shader and some do not.
     */
    GLint nbShaders = getNbAttachedShaders(this->_shaderProgramId);
    if (error.hasOccured())
    {
        return LinkResult{false, error.toString("Cannot retrieve attached shaders")};
    }
    if (!nbShaders)
    {
        return LinkResult{false, "Cannot link program because no shader is attached!"};
    }

    sys::Duration duration;
    glLinkProgram(_shaderProgramId);
    unsigned long linkageDuration = duration.elapsed();
    if (error)
    {
        return LinkResult{false, error.toString("Cannot link program")};
    }

    GLint linkStatus = GL_FALSE;
    glGetProgramiv(_shaderProgramId, GL_LINK_STATUS, &linkStatus);

    return LinkResult{linkStatus == GL_TRUE, extractInfoLog(_shaderProgramId), linkageDuration};
}

ValidationResult ShaderProgram::validate()
{
    GlError error;

    GLint linkStatus = GL_FALSE;
    glGetProgramiv(_shaderProgramId, GL_LINK_STATUS, &linkStatus);
    if (!linkStatus)
    {
        return ValidationResult{false, "Cannot validate unlinked shader program"};
    }

    glValidateProgram(_shaderProgramId);
    if (error)
    {
        return ValidationResult{false, error.toString("Cannot validate shader program")};
    }

    GLint validationStatus = GL_FALSE;
    glGetProgramiv(_shaderProgramId, GL_VALIDATE_STATUS, &validationStatus);

    return ValidationResult{validationStatus == GL_TRUE, extractInfoLog(_shaderProgramId)};
}

UniformDeclarationVector ShaderProgram::getUniformDeclarations() const
{
    UniformDeclarationVector vector;
    GlError glError;
    GLint nbUniforms = 0;
    glGetProgramiv(_shaderProgramId, GL_ACTIVE_UNIFORMS, &nbUniforms);
    if (glError)
    {
        return vector;
    }
    if (nbUniforms > 0)
    {
        GLint activeUniformMaxLength = 0;
        glGetProgramiv(_shaderProgramId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &activeUniformMaxLength);
        if (glError || activeUniformMaxLength <= 0)
        {
            return vector;
        }
        auto activeUniformName = std::unique_ptr<GLchar[]>(new GLchar[activeUniformMaxLength]);
        for (int i = 0; i < nbUniforms; ++i)
        {
            GLint activeUniformSize = 0;
            GLenum activeUniformType = 0;
            glGetActiveUniform(_shaderProgramId, i, activeUniformMaxLength, 0, &activeUniformSize, &activeUniformType, activeUniformName.get());
            if (glError) {
                vector.clear();
                break;
            }
            GLint uniformLocation = glGetUniformLocation(this->_shaderProgramId, activeUniformName.get());
            if (uniformLocation >= 0)
            {
                vector.push_back(UniformDeclaration{_shaderProgramId, uniformLocation, activeUniformSize, activeUniformType, activeUniformName.get()});
            }
        }
    }
    return vector;
}

UniformDeclaration ShaderProgram::getActiveUniform(const char *name) const
{
    GlError glError;
    GLenum activeUniformType = 0;
    GLint activeUniformSize = 0;
    GLint uniformLocation = glGetUniformLocation(_shaderProgramId, name);

    if(!glError && uniformLocation >= 0)
    {
        GLuint uniformIndex = 0u;
        glGetUniformIndices(_shaderProgramId, 1, &name, &uniformIndex);
        if (!glError && uniformIndex != GL_INVALID_INDEX)
        {
            char tmp = 0;
            glGetActiveUniform(_shaderProgramId, uniformIndex, 0, 0, &activeUniformSize, &activeUniformType, &tmp);
        }
    }
    return UniformDeclaration{_shaderProgramId, uniformLocation, activeUniformSize, activeUniformType, name};
}

VertexAttributeDeclarationVector ShaderProgram::getVertexAttributeDeclarations() const
{
    VertexAttributeDeclarationVector vector;
    GlError glError;
    GLint nbAttributes = 0;
    glGetProgramiv(_shaderProgramId, GL_ACTIVE_ATTRIBUTES, &nbAttributes);
    if (glError)
    {
        return vector;
    }
    if (nbAttributes > 0)
    {
        GLint activeAttributeMaxLength = 0;
        glGetProgramiv(_shaderProgramId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &activeAttributeMaxLength);
        if (glError || activeAttributeMaxLength <= 0)
        {
            return vector;
        }
        auto activeAttributeName = std::unique_ptr<GLchar[]>(new GLchar[activeAttributeMaxLength]);
        for (int i = 0; i < nbAttributes; ++i)
        {
            GLint activeAttributeSize = 0;
            GLenum activeAttributeType = 0;
            glGetActiveAttrib(_shaderProgramId, i, activeAttributeMaxLength, NULL, &activeAttributeSize, &activeAttributeType, activeAttributeName.get());
            if (glError) {
                vector.clear();
                break;
            }
            GLint attributeLocation = glGetAttribLocation(_shaderProgramId, activeAttributeName.get());
            if (glError) {
                vector.clear();
                break;
            }
            vector.push_back(VertexAttributeDeclaration{attributeLocation, activeAttributeSize, activeAttributeType, activeAttributeName.get()});
        }
    }
    return vector;
}

void ShaderProgram::deleteShaderProgram()
{
    if (_shaderProgramId != 0)
    {
        glDeleteProgram(_shaderProgramId);
        _shaderProgramId = 0;
    }
}
