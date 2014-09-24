#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "gl.hpp"

#include "Shader.hpp"
#include "UniformDeclaration.hpp"
#include "OperationResult.hpp"

namespace glv
{

typedef OperationResult ShaderAttachmentResult;
typedef OperationResult LinkResult;
typedef OperationResult ValidationResult;

class ShaderProgram
{
public:
    ShaderProgram();
    ShaderProgram(const ShaderProgram& shaderProgram);
    ~ShaderProgram();

    ShaderProgram& operator = (const ShaderProgram& shaderProgram);

    ShaderAttachmentResult attach(const Shader& shader);

    bool has(const Shader& shader) const;

    ShaderAttachmentResult detach(const Shader& shader);

    void detachAllShaders();

    LinkResult link();

    ValidationResult validate();

    void extractActive(UniformDeclarationVector& vector) const;

    UniformDeclaration getActiveUniform(const char *name) const;

    void extractActive(VertexAttributeDeclarationVector& vector) const;

    inline GLuint getId() const
    {
        return _shaderProgramId;
    }

    bool exists() const;

    void use() const
    {
        glUseProgram(_shaderProgramId);
    }

private:
    void attachShadersFrom(const ShaderProgram& shaderProgram);
    void deleteShaderProgram();
    GLuint getNbAttachedShaders()const;
    GLuint* getAttachedShaders()const;

    GLuint _shaderProgramId;
};

}


#endif // SHADERPROGRAM_H
