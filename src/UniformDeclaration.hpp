#ifndef UNIFORMDECLARATION_H
#define UNIFORMDECLARATION_H

#include <string>
#include <vector>
#include "gl.hpp"

namespace glv
{

class UniformDeclaration
{
public:
    UniformDeclaration(GLuint index, GLint size, GLenum type, const char* name);
    UniformDeclaration(const UniformDeclaration& uniformDeclaration);

    UniformDeclaration& operator = (const UniformDeclaration& uniformDeclaration);

    bool operator == (const UniformDeclaration& uniformDeclaration) const;

    inline bool operator != (const UniformDeclaration& uniformDeclaration) const
    {
        return !(*this == uniformDeclaration);
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

typedef std::vector<UniformDeclaration> UniformDeclarationVector;

}

#endif // UNIFORMDECLARATION_H
