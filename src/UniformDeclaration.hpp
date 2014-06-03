#ifndef ShaderValueDeclaration_H
#define ShaderValueDeclaration_H

#include <string>
#include <vector>
#include "gl.hpp"

namespace glv
{

class ShaderValueDeclaration
{
protected:

    ShaderValueDeclaration(GLuint index, GLint size, GLenum type, const char *name);

public:

    bool operator == (const ShaderValueDeclaration &svd) const;

    inline bool operator != (const ShaderValueDeclaration &svd) const
    {
        return !(this->operator ==(svd));
    }

    inline const std::string& getName() const
    {
        return _name;
    }

    inline GLuint getIndex() const
    {
        return _index;
    }

    inline GLint getSize() const
    {
        return _size;
    }

    inline GLenum getType() const
    {
        return _type;
    }

    inline bool isArray() const
    {
        return _size > 1;
    }

private:
    void normalizeArrayName();
    GLuint _index;
    GLint _size;
    GLenum _type;
    std::string _name;
};

class UniformDeclaration : public ShaderValueDeclaration
{
public:
    UniformDeclaration(GLuint index, GLint size, GLenum type, const char *name);
};

typedef std::vector<UniformDeclaration> UniformDeclarationVector;

class VertexAttributeDeclaration : public ShaderValueDeclaration
{
public:
    VertexAttributeDeclaration(GLuint index, GLint size, GLenum type, const char *name);
};

typedef std::vector<VertexAttributeDeclaration> VertexAttributeDeclarationVector;

}

#endif // ShaderValueDeclaration_H
