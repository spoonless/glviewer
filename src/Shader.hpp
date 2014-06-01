#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "gl.hpp"

namespace glv
{

class Shader
{
public:
    enum ShaderType {VERTEX_SHADER, GEOMETRY_SHADER, FRAGMENT_SHADER};

    Shader(ShaderType type);
    Shader(const Shader& shader);
    ~Shader();
    Shader& operator = (const Shader& shader);

    inline GLuint getId() const
    {
        return _shaderId;
    }

    bool exists() const;

    inline ShaderType getType() const
    {
        return _type;
    }

    void extractSource(std::string& source) const;

    inline unsigned int getCompilationDuration() const
    {
        return _compilationDuration;
    }

    bool compile(const std::string &source)
    {
        return compile(source.c_str());
    }

    bool compile(const char* source);

    inline const std::string& getLastCompilationLog() const
    {
        return _lastCompilationLog;
    }

private:
    void deleteShaderId();
    void extractInfoLog();
    void createShader();

    GLuint _shaderId;
    const ShaderType _type;
    unsigned int _compilationDuration;
    std::string _lastCompilationLog;
};

}


#endif // SHADER_H
