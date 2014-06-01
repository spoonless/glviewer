#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <string>
#include "gl.hpp"

#include "Shader.hpp"
#include "UniformDeclaration.hpp"

namespace glv
{

class ShaderProgram
{
public:
    ShaderProgram();
    ShaderProgram(const ShaderProgram& shaderProgram);
    ~ShaderProgram();

    ShaderProgram& operator = (const ShaderProgram& shaderProgram);

    bool attach(const Shader& shader);

    bool has(const Shader& shader) const;

    bool detach(const Shader& shader);

    void detachAllShaders();

    bool link();

    bool validate();

    void extractActive(UniformDeclarationVector& vector);

    inline const std::string& getLastLinkLog() const
    {
        return _lastLinkLog;
    }

    inline const std::string& getLastValidationLog() const
    {
        return _lastValidationLog;
    }

    inline GLuint getId() const
    {
        return _shaderProgramId;
    }

    bool exists() const;

    inline unsigned long getLinkageDuration() const
    {
        return _linkageDuration;
    }

private:
    void attachShadersFrom(const ShaderProgram& shaderProgram);
    void extractInfoLog(std::string &log);
    void deleteShaderProgram();
    GLuint getNbAttachedShaders()const;
    GLuint* getAttachedShaders()const;

    GLuint _shaderProgramId;
    unsigned long _linkageDuration;
    std::string _lastLinkLog;
    std::string _lastValidationLog;
};

}


#endif // SHADERPROGRAM_H
