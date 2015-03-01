#include <cstring>
#include <string>
#include "Duration.hpp"
#include "ShaderProgram.hpp"
#include "GlError.hpp"

using namespace glv;

namespace
{

void extractInfoLog(GLuint shaderProgramId, std::string& log)
{
    GlError error;
    GLint infoLogLength = 0;

    glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);

    if (infoLogLength == 0)
    {
        log.clear();
        return;
    }

    char* infoLogBuffer = new char[infoLogLength];
    glGetProgramInfoLog(shaderProgramId, infoLogLength, NULL, infoLogBuffer);
    if (error.hasOccured())
    {
        log = error.toString("Cannot retrieve properly shader program link log info");
    }
    else
    {
        log = infoLogBuffer;
    }
    delete[] infoLogBuffer;
}

}

ShaderProgram::ShaderProgram()
    : _shaderProgramId(glCreateProgram())
{
}

ShaderProgram::ShaderProgram(const ShaderProgram& shaderProgram)
    : _shaderProgramId(glCreateProgram())
{
    attachShadersFrom(shaderProgram);
}

ShaderProgram::~ShaderProgram()
{
    deleteShaderProgram();
}

ShaderProgram& ShaderProgram::operator = (const ShaderProgram& shaderProgram)
{
    if (this != &shaderProgram)
    {
        detachAllShaders();
        attachShadersFrom(shaderProgram);
    }
    return *this;
}

bool ShaderProgram::exists() const
{
    return _shaderProgramId != 0 && glIsProgram(_shaderProgramId);
}

ShaderAttachmentResult ShaderProgram::attach(const Shader& shader)
{
    if (!shader.exists())
    {
        return ShaderAttachmentResult(false, "Attempt to attach a non shader object to GLSL program!");
    }
    GlError error;
    glAttachShader(_shaderProgramId, shader.getId());
    return ShaderAttachmentResult(!error, error ? error.toString("Error while attempting to attach shader object to GLSL program") : "");
}

bool ShaderProgram::has(const Shader& shader) const
{
    if (!shader.exists())
    {
        return false;
    }

    GLuint* shaders = getAttachedShaders();
    bool found = false;

    for(int i = 0; !found && shaders[i] != 0; ++i)
    {
        found = shaders[i] == shader.getId();
    }
    delete[] shaders;

    return found;
}

ShaderAttachmentResult ShaderProgram::detach(const Shader& shader)
{
    if (!shader.exists())
    {
        return ShaderAttachmentResult(false, "Attempt to detach a non shader object to GLSL program!");
    }
    GlError error;
    glDetachShader(_shaderProgramId, shader.getId());
    return ShaderAttachmentResult(!error, error ? error.toString("Error while attempting to detach shader object to GLSL program"): "");
}

void ShaderProgram::detachAllShaders()
{
    GLuint* shaders = getAttachedShaders();
    for(int i = 0; shaders[i] != 0; ++i)
    {
        glDetachShader(_shaderProgramId, shaders[i]);
    }
    delete[] shaders;
}

LinkResult ShaderProgram::link()
{
    GlError error;

    /*
     * Checking programmatically that at least one shader is attached to this program.
     * NVidia and AMD cards have heterogeneous behaviors. Some allow link operation
     * for program with no attached shader and some do not.
     */
    GLint nbShaders = getNbAttachedShaders();
    if (error.hasOccured())
    {
        return LinkResult(false, error.toString("Cannot retrieve attached shaders"));
    }
    if (!nbShaders)
    {
        return LinkResult(false, "Cannot link program because no shader is attached!");
    }

    sys::Duration duration;
    glLinkProgram(_shaderProgramId);
    unsigned long linkageDuration = duration.elapsed();
    if (error)
    {
        return LinkResult(false, error.toString("Cannot link program"));
    }

    GLint linkStatus = GL_FALSE;
    glGetProgramiv(_shaderProgramId, GL_LINK_STATUS, &linkStatus);

    std::string lastLinkLog;
    extractInfoLog(_shaderProgramId, lastLinkLog);

	return LinkResult(linkStatus == GL_TRUE, lastLinkLog, linkageDuration);
}

ValidationResult ShaderProgram::validate()
{
    GlError error;

    GLint linkStatus = GL_FALSE;
    glGetProgramiv(_shaderProgramId, GL_LINK_STATUS, &linkStatus);
    if (!linkStatus)
    {
        return ValidationResult(false, "Cannot validate unlinked shader program");
    }

    glValidateProgram(_shaderProgramId);
    if (error)
    {
        return ValidationResult(false, error.toString("Cannot validate shader program"));
    }

    GLint validationStatus = GL_FALSE;
    glGetProgramiv(_shaderProgramId, GL_VALIDATE_STATUS, &validationStatus);

    std::string validationLog;

    extractInfoLog(_shaderProgramId, validationLog);

	return ValidationResult(validationStatus == GL_TRUE, validationLog);
}

void ShaderProgram::extractActive(UniformDeclarationVector& vector) const
{
    vector.clear();
    GlError glError;
    GLint nbUniforms = 0;
    glGetProgramiv(_shaderProgramId, GL_ACTIVE_UNIFORMS, &nbUniforms);
    if (glError)
    {
        return;
    }
    if (nbUniforms > 0)
    {
        GLint activeUniformMaxLength = 0;
        glGetProgramiv(_shaderProgramId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &activeUniformMaxLength);
        if (glError || activeUniformMaxLength <= 0)
        {
            return;
        }
        char* activeUniformName = new char[activeUniformMaxLength];
        for (int i = 0; i < nbUniforms; ++i)
        {
            GLint activeUniformSize = 0;
            GLenum activeUniformType = 0;
            glGetActiveUniform(_shaderProgramId, i, activeUniformMaxLength, 0, &activeUniformSize, &activeUniformType, activeUniformName);
            if (glError) {
                vector.clear();
                break;
            }
            GLint uniformLocation = glGetUniformLocation(this->_shaderProgramId, activeUniformName);
            if (uniformLocation >= 0)
            {
                vector.push_back(UniformDeclaration(_shaderProgramId, uniformLocation, activeUniformSize, activeUniformType, activeUniformName));
            }
        }
        delete[]activeUniformName;
    }
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
    return UniformDeclaration(_shaderProgramId, uniformLocation, activeUniformSize, activeUniformType, name);
}

void ShaderProgram::extractActive(VertexAttributeDeclarationVector& vector) const
{
    vector.clear();
    GlError glError;
    GLint nbAttributes = 0;
    glGetProgramiv(_shaderProgramId, GL_ACTIVE_ATTRIBUTES, &nbAttributes);
    if (glError)
    {
        return;
    }
    if (nbAttributes > 0)
    {
        GLint activeAttributeMaxLength = 0;
        glGetProgramiv(_shaderProgramId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &activeAttributeMaxLength);
        if (glError || activeAttributeMaxLength <= 0)
        {
            return;
        }
        char* activeAttributeName = new char[activeAttributeMaxLength];
        for (int i = 0; i < nbAttributes; ++i)
        {
            GLint activeAttributeSize = 0;
            GLenum activeAttributeType = 0;
            glGetActiveAttrib(_shaderProgramId, i, activeAttributeMaxLength, NULL, &activeAttributeSize, &activeAttributeType, activeAttributeName);
            if (glError) {
                vector.clear();
                break;
            }
            GLuint attributeLocation = glGetAttribLocation(_shaderProgramId, activeAttributeName);
            if (glError) {
                vector.clear();
                break;
            }
            vector.push_back(VertexAttributeDeclaration(attributeLocation, activeAttributeSize, activeAttributeType, activeAttributeName));
        }
        delete[]activeAttributeName;
    }
}

void ShaderProgram::attachShadersFrom(const ShaderProgram& shaderProgram)
{
    GLuint* shaders = shaderProgram.getAttachedShaders();
    for (int i = 0; shaders[i] != 0; ++i)
    {
        glAttachShader(_shaderProgramId, shaders[i]);
    }
    delete[] shaders;
}

void ShaderProgram::deleteShaderProgram()
{
    if (_shaderProgramId != 0)
    {
        glDeleteProgram(_shaderProgramId);
        _shaderProgramId = 0;
    }
}

GLuint ShaderProgram::getNbAttachedShaders()const
{
    GLint nbShaders = 0;
    glGetProgramiv(_shaderProgramId, GL_ATTACHED_SHADERS, &nbShaders);
    return nbShaders;
}

GLuint* ShaderProgram::getAttachedShaders() const
{
    GLint nbShaders = getNbAttachedShaders();

    GLuint* shaders = new GLuint[nbShaders + 1];

    GLsizei count = 0;
    glGetAttachedShaders(_shaderProgramId, nbShaders, &count, shaders);
    shaders[count] = 0;

    return shaders;
}


