#include <iostream>

#include "Shader.hpp"
#include "GlError.hpp"
#include "Duration.hpp"

using namespace glv;

namespace {

void extractInfoLog(GLuint shaderId, std::string& out)
{
    GlError error;
    GLint infoLogLength = 0;

    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

    if (infoLogLength == 0)
    {
        out.clear();
        return;
    }

    char* infoLogBuffer = new char[infoLogLength];
    glGetShaderInfoLog(shaderId, infoLogLength, NULL, infoLogBuffer);

    if (error.hasOccured())
    {
        out = error.toString("Cannot retrieve properly shader compilation log info");
    }
    else
    {
        out = infoLogBuffer;
    }
    delete[] infoLogBuffer;
}

}

Shader::Shader(ShaderType type) :
    _shaderId(0), _type(type)
{
    createShader();
}

Shader::Shader(const Shader& shader) :
    _shaderId(0), _type(shader._type)
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

CompilationResult Shader::compile(const char* source)
{
    GlError error;

    if (source[0] == '\0')
    {
        return CompilationResult(false, "Shader source is empty!");
    }

    glShaderSource(_shaderId, 1, &source, NULL);
    if (error.hasOccured())
    {
        return CompilationResult(false, error.toString("Error while attaching source to shader (glShaderSource)"));
    }

    sys::Duration duration;
    glCompileShader(_shaderId);
    unsigned int compilationDuration = duration.elapsed();
    if (error.hasOccured())
    {
        return CompilationResult(false, error.toString("Error while compiling shader (glCompileShader)"));
    }

    GLint compilationSucceeded = GL_FALSE;
    glGetShaderiv(_shaderId, GL_COMPILE_STATUS, &compilationSucceeded);
    std::string log;
    extractInfoLog(_shaderId, log);

    return CompilationResult(compilationSucceeded == GL_TRUE, log, compilationDuration);
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
    }
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
