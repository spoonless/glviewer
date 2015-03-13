#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "gl.hpp"
#include "Shader.hpp"
#include "UniformDeclaration.hpp"
#include "OperationResult.hpp"

namespace glv
{

using ShaderAttachment = OperationResult;
using ShaderLink = OperationResult;
using ShaderValidation = OperationResult;

class ShaderProgram
{
public:
    ShaderProgram();
    ShaderProgram(ShaderProgram &&shaderProgram);
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram &shaderProgram) = delete;
    ShaderProgram& operator = (const ShaderProgram &shaderProgram) = delete;

    ShaderAttachment attach(const Shader &shader);

    bool has(const Shader &shader) const;

    ShaderAttachment detach(const Shader &shader);

    void detachAllShaders();

    ShaderLink link();

    ShaderValidation validate();

    UniformDeclarationVector getUniformDeclarations() const;

    UniformDeclaration getActiveUniform(const char *name) const;

    VertexAttributeDeclarationVector getVertexAttributeDeclarations() const;

    inline GLuint getId() const
    {
        return _shaderProgramId;
    }

    bool exists() const;

    inline void use() const
    {
        glUseProgram(_shaderProgramId);
    }

private:
    void deleteShaderProgram();

    GLuint _shaderProgramId;
};

}

#endif // SHADERPROGRAM_H
