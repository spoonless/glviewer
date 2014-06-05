#include <cstring>
#include "Duration.hpp"
#include "ShaderProgram.hpp"
#include "GlError.hpp"

using namespace glv;

ShaderProgram::ShaderProgram()
    : _shaderProgramId(glCreateProgram()), _linkageDuration(0)
{
}

ShaderProgram::ShaderProgram(const ShaderProgram& shaderProgram)
    : _shaderProgramId(glCreateProgram()), _linkageDuration(0)
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

bool ShaderProgram::attach(const Shader& shader)
{
    if (!shader.exists())
    {
        return false;
    }
    GlError error;
    glAttachShader(_shaderProgramId, shader.getId());
    return !error.hasOccured();
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

bool ShaderProgram::detach(const Shader& shader)
{
    if (!shader.exists())
    {
        return false;
    }
    GlError error;
    glDetachShader(_shaderProgramId, shader.getId());
    return !error.hasOccured();
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

bool ShaderProgram::link()
{
    _lastLinkLog.clear();
    _linkageDuration = 0;
    GlError error;

    /*
     * Checking programmatically that at least one shader is attached to this program.
     * NVidia and AMD cards have heterogeneous behaviors. Some allow link operation
     * for program with no attached shader and some do not.
     */
    GLint nbShaders = getNbAttachedShaders();
    if (error.hasOccured())
    {
        _lastLinkLog = error.toString("Cannot retrieve attached shaders");
        return false;
    }
    if (!nbShaders)
    {
        _lastLinkLog = "Cannot link program because no shader is attached!";
        return false;
    }

    Duration duration;
    glLinkProgram(_shaderProgramId);
    _linkageDuration = duration.elapsed();
    if (error.hasOccured())
    {
        _lastLinkLog = error.toString("Cannot link program");
        return false;
    }

    GLint linkStatus = GL_FALSE;
    glGetProgramiv(_shaderProgramId, GL_LINK_STATUS, &linkStatus);

    extractInfoLog(_lastLinkLog);

    return linkStatus == GL_TRUE;
}

bool ShaderProgram::validate()
{
    _lastLinkLog.clear();
    GlError error;

    GLint linkStatus = GL_FALSE;
    glGetProgramiv(_shaderProgramId, GL_LINK_STATUS, &linkStatus);
    if (!linkStatus)
    {
        _lastValidationLog = "Cannot validate unlinked shader program";
        return false;
    }

    glValidateProgram(_shaderProgramId);
    if (error.hasOccured())
    {
        _lastValidationLog = error.toString("Cannot validate shader program");
        return false;
    }

    GLint validationStatus = GL_FALSE;
    glGetProgramiv(_shaderProgramId, GL_VALIDATE_STATUS, &validationStatus);

    extractInfoLog(_lastValidationLog);

    return validationStatus == GL_TRUE;
}

void ShaderProgram::extractActive(UniformDeclarationVector& vector)
{
    vector.clear();
    GlError glError;
    GLint nbUniforms = 0;
    glGetProgramiv(_shaderProgramId, GL_ACTIVE_UNIFORMS, &nbUniforms);
    if (glError.hasOccured())
    {
        return;
    }
    if (nbUniforms > 0)
    {
        GLint activeUniformMaxLength = 0;
        glGetProgramiv(_shaderProgramId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &activeUniformMaxLength);
        if (glError.hasOccured() || activeUniformMaxLength <= 0)
        {
            return;
        }
        char* activeUniformName = new char[activeUniformMaxLength];
        for (int i = 0; i < nbUniforms; ++i)
        {
            GLint activeUniformSize = 0;
            GLenum activeUniformType = 0;
            glGetActiveUniform(_shaderProgramId, i, activeUniformMaxLength, 0, &activeUniformSize, &activeUniformType, activeUniformName);
            if (glError.hasOccured()) {
                vector.clear();
                break;
            }
            if (strncmp(activeUniformName, "gl_", 3))
            {
                vector.push_back(UniformDeclaration(i, activeUniformSize, activeUniformType, activeUniformName));
            }
        }
        delete[]activeUniformName;
    }
}

UniformDeclaration ShaderProgram::getActiveUniform(const char *name) const
{
    GLint location = glGetUniformLocation(this->_shaderProgramId, name);
    if (location == -1)
    {
        return UniformDeclaration();
    }
    GLint activeUniformSize = 0;
    GLenum activeUniformType = 0;
    glGetActiveUniform(_shaderProgramId, location, 0, 0, &activeUniformSize, &activeUniformType, 0);

    return UniformDeclaration(location, activeUniformSize, activeUniformType, name);

}

void ShaderProgram::extractActive(VertexAttributeDeclarationVector& vector)
{
    vector.clear();
    GlError glError;
    GLint nbAttributes = 0;
    glGetProgramiv(_shaderProgramId, GL_ACTIVE_ATTRIBUTES, &nbAttributes);
    if (glError.hasOccured())
    {
        return;
    }
    if (nbAttributes > 0)
    {
        GLint activeAttributeMaxLength = 0;
        glGetProgramiv(_shaderProgramId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &activeAttributeMaxLength);
        if (glError.hasOccured() || activeAttributeMaxLength <= 0)
        {
            return;
        }
        char* activeAttributeName = new char[activeAttributeMaxLength];
        for (int i = 0; i < nbAttributes; ++i)
        {
            GLint activeAttributeSize = 0;
            GLenum activeAttributeType = 0;
            glGetActiveAttrib(_shaderProgramId, i, activeAttributeMaxLength, NULL, &activeAttributeSize, &activeAttributeType, activeAttributeName);
            if (glError.hasOccured()) {
                vector.clear();
                break;
            }
            vector.push_back(VertexAttributeDeclaration(i, activeAttributeSize, activeAttributeType, activeAttributeName));
        }
        delete[]activeAttributeName;
    }
}

void ShaderProgram::extractInfoLog(std::string& log)
{
    GlError error;
    GLint infoLogLength = 0;

    glGetProgramiv(_shaderProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);

    char* infoLogBuffer = new char[infoLogLength];
    glGetProgramInfoLog(_shaderProgramId, infoLogLength, NULL, infoLogBuffer);
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


