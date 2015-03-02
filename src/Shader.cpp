#include <memory>
#include <iostream>

#include "Shader.hpp"
#include "GlError.hpp"
#include "Duration.hpp"

using namespace glv;

namespace {

std::string getInfoLog(GLuint shaderId)
{
    GlError error;
    GLint infoLogLength = 0;

    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

    if (infoLogLength == 0)
    {
        return std::string{};
    }

    auto infoLogBuffer = std::unique_ptr<GLchar[]>(new GLchar[infoLogLength]);
    glGetShaderInfoLog(shaderId, infoLogLength, NULL, infoLogBuffer.get());

    if (error.hasOccured())
    {
        return error.toString("Cannot retrieve properly shader compilation log info");
    }
    else
    {
        return std::string{infoLogBuffer.get()};
    }
}

}

Shader::Shader(ShaderType type) :
    _shaderId{0}, _type{type}
{
    createShader();
}

Shader::Shader(Shader &&shader) :
    _shaderId{shader._shaderId}, _type{shader._type}
{
    shader._shaderId = 0;
}

Shader::~Shader()
{
    deleteShader();
}

bool Shader::exists() const
{
    return _shaderId != 0 && glIsShader(_shaderId);
}

std::string Shader::getSource() const
{
    GlError error;

    if (_shaderId == 0)
    {
        return std::string{};
    }

    GLint sourceLength = 0;
    glGetShaderiv(_shaderId, GL_SHADER_SOURCE_LENGTH, &sourceLength);
    if (error.hasOccured())
    {
        std::cerr << error.toString("Error while retrieving shader source length (glGetShaderiv(_shaderId, GL_SHADER_SOURCE_LENGTH, sourceLength))") << std::endl;
    }

    if (sourceLength == 0)
    {
        return std::string{};
    }

    auto sourceBuffer = std::unique_ptr<GLchar[]>(new GLchar[sourceLength]);
    glGetShaderSource(_shaderId, sourceLength, NULL, sourceBuffer.get());
    if (error.hasOccured())
    {
        std::cerr << error.toString("Error while retrieving shader source (glGetShaderSource)") << std::endl;
        return std::string{};
    }
    else
    {
        return std::string{sourceBuffer.get()};
    }
}

CompilationResult Shader::compile(const char *source)
{
    GlError error;

    if (source[0] == '\0')
    {
        return CompilationResult{false, "Shader source is empty!"};
    }

    glShaderSource(_shaderId, 1, &source, NULL);
    if (error.hasOccured())
    {
        return CompilationResult{false, error.toString("Error while attaching source to shader (glShaderSource)")};
    }

    sys::Duration duration;
    glCompileShader(_shaderId);
    unsigned int compilationDuration = duration.elapsed();
    if (error.hasOccured())
    {
        return CompilationResult{false, error.toString("Error while compiling shader (glCompileShader)")};
    }

    GLint compilationSucceeded = GL_FALSE;
    glGetShaderiv(_shaderId, GL_COMPILE_STATUS, &compilationSucceeded);

    return CompilationResult{compilationSucceeded == GL_TRUE, getInfoLog(_shaderId), compilationDuration};
}

void Shader::deleteShader()
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
    case ShaderType::VERTEX_SHADER:
        _shaderId = glCreateShader(GL_VERTEX_SHADER);
        break;
    case ShaderType::GEOMETRY_SHADER:
        _shaderId = glCreateShader(GL_GEOMETRY_SHADER);
        break;
    case ShaderType::FRAGMENT_SHADER:
        _shaderId = glCreateShader(GL_FRAGMENT_SHADER);
        break;
    }
}
