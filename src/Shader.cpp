#include <iostream>

#include "Shader.hpp"
#include "GlError.hpp"
#include "Duration.hpp"

using namespace glv;

Shader::Shader(ShaderType type) :
    _shaderId(0), _type(type), _compilationDuration(0)
{
    createShader();
}

Shader::Shader(const Shader& shader) :
    _shaderId(0), _type(shader._type), _compilationDuration(0)
{
    createShader();
    std::string source;
    shader.extractSource(source);
    if (!source.empty())
    {
        compile(source.c_str());
    }
}

Shader::~Shader()
{
    deleteShaderId();
}

Shader& Shader::operator = (const Shader& shader)
{
    if (this != &shader && _type == shader._type)
    {
        std::string source;
        shader.extractSource(source);
        if (!source.empty()){
            compile(source.c_str());
        }
    }
    return *this;
}

bool Shader::exists() const
{
    return _shaderId != 0 && glIsShader(_shaderId);
}

void Shader::extractSource(std::string& source) const
{
    GlError error;
    source.clear();

    if (_shaderId == 0)
    {
        return;
    }

    GLint sourceLength = 0;
    glGetShaderiv(_shaderId, GL_SHADER_SOURCE_LENGTH, &sourceLength);
    if (error.hasOccured())
    {
        std::cerr << error.toString("Error while retrieving shader source length (glGetShaderiv(_shaderId, GL_SHADER_SOURCE_LENGTH, sourceLength))") << std::endl;
    }

    if (sourceLength == 0)
    {
        return;
    }

    GLchar* sourceBuffer = new GLchar[sourceLength];
    glGetShaderSource(_shaderId, sourceLength, NULL, sourceBuffer);
    if (error.hasOccured())
    {
        std::cerr << error.toString("Error while retrieving shader source (glGetShaderSource)") << std::endl;
    }
    else
    {
        source = sourceBuffer;
    }
    delete[] sourceBuffer;
}

bool Shader::compile(const char* source)
{
    _compilationDuration = 0;
    GlError error;

    if (source[0] == '\0')
    {
        _lastCompilationLog = "Shader source is empty!";
        return false;
    }

    glShaderSource(_shaderId, 1, &source, NULL);
    if (error.hasOccured())
    {
        _lastCompilationLog = error.toString("Error while attaching source to shader (glShaderSource)");
        return false;
    }

    Duration duration;
    glCompileShader(_shaderId);
    _compilationDuration= duration.elapsed();
    if (error.hasOccured())
    {
        _lastCompilationLog = error.toString("Error while compiling shader (glCompileShader)");
        return false;
    }

    GLint compilationSucceeded = GL_FALSE;
    glGetShaderiv(_shaderId, GL_COMPILE_STATUS, &compilationSucceeded);

    extractInfoLog();

    return compilationSucceeded == GL_TRUE;
}

void Shader::deleteShaderId()
{
    if (_shaderId != 0)
    {
        GlError error;
        glDeleteShader(_shaderId);
        if (error.hasOccured())
        {
            std::cerr << error.toString("Error while deleting shader (glDeleteShader)") << std::endl;
        }
        _shaderId = 0;
        _compilationDuration = 0;
        _lastCompilationLog.clear();
    }
}

void Shader::extractInfoLog()
{
    GlError error;
    GLint infoLogLength = 0;

    glGetShaderiv(_shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

    char* infoLogBuffer = new char[infoLogLength];
    glGetShaderInfoLog(_shaderId, infoLogLength, NULL, infoLogBuffer);
    if (error.hasOccured())
    {
        _lastCompilationLog = error.toString("Cannot retrieve properly shader compilation log info");
    }
    else
    {
        _lastCompilationLog = infoLogBuffer;
    }
    delete[] infoLogBuffer;
}

void Shader::createShader()
{
    switch (_type) {
    case VERTEX_SHADER:
        _shaderId = glCreateShader(GL_VERTEX_SHADER);
        break;
    case GEOMETRY_SHADER:
        _shaderId = glCreateShader(GL_GEOMETRY_SHADER);
        break;
    case FRAGMENT_SHADER:
        _shaderId = glCreateShader(GL_FRAGMENT_SHADER);
        break;
    }
}
