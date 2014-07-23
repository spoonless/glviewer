#ifndef SHADER_H
#define SHADER_H

#include "gl.hpp"
#include "OperationResult.hpp"

namespace glv
{

typedef OperationResult CompilationResult;

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

    CompilationResult compile(const std::string &source)
    {
        return compile(source.c_str());
    }

    CompilationResult compile(const char* source);

private:
    void deleteShaderId();
    void createShader();

    GLuint _shaderId;
    const ShaderType _type;
};

}


#endif // SHADER_H
