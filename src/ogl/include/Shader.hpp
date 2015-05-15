#ifndef SHADER_H
#define SHADER_H

#include "gl.hpp"
#include "OperationResult.hpp"

namespace ogl
{

using ShaderCompilation = sys::OperationResult;

enum class ShaderType {VERTEX_SHADER, GEOMETRY_SHADER, FRAGMENT_SHADER};

class Shader
{
public:

    explicit Shader(ShaderType type);
    Shader(Shader &&shader);
    ~Shader();

    Shader(const Shader &shader) = delete;
    Shader& operator = (const Shader &shader) = delete;

    inline GLuint getId() const
    {
        return _shaderId;
    }

    bool exists() const;

    inline ShaderType getType() const
    {
        return _type;
    }

    std::string getSource() const;

    ShaderCompilation compile(const std::string &source)
    {
        return compile(source.c_str());
    }

    ShaderCompilation compile(const char *source);

private:
    void deleteShader();
    void createShader();

    GLuint _shaderId;
    const ShaderType _type;
};

}


#endif // SHADER_H
