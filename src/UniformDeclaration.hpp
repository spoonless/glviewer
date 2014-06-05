#ifndef ShaderValueDeclaration_H
#define ShaderValueDeclaration_H

#include <string>
#include <vector>
#include "gl.hpp"

#include "glm/fwd.hpp"

namespace glv
{

class UniformBinder
{
public:
    UniformBinder(GLint location) : _location(location)
    {
    }

    UniformBinder& operator = (const glm::f32 &v);
    UniformBinder& operator = (const glm::fvec2 &v);
    UniformBinder& operator = (const glm::fvec3 &v);
    UniformBinder& operator = (const glm::fvec4 &v);

    UniformBinder& operator = (const glm::i32 &v);
    UniformBinder& operator = (const glm::i32vec2 &v);
    UniformBinder& operator = (const glm::i32vec3 &v);
    UniformBinder& operator = (const glm::i32vec4 &v);

    UniformBinder& operator = (const glm::u32 &v);
    UniformBinder& operator = (const glm::u32vec2 &v);
    UniformBinder& operator = (const glm::u32vec3 &v);
    UniformBinder& operator = (const glm::u32vec4 &v);
    UniformBinder& operator = (const glm::f32mat2 &v);
    UniformBinder& operator = (const glm::f32mat3 &v);
    UniformBinder& operator = (const glm::f32mat4 &v);
    UniformBinder& operator = (const glm::f32mat2x3 &v);
    UniformBinder& operator = (const glm::f32mat3x2 &v);
    UniformBinder& operator = (const glm::f32mat2x4 &v);
    UniformBinder& operator = (const glm::f32mat4x2 &v);
    UniformBinder& operator = (const glm::f32mat3x4 &v);
    UniformBinder& operator = (const glm::f32mat4x3 &v);

private:
    GLint _location;
};


class ShaderValueDeclaration
{
protected:

    ShaderValueDeclaration();
    ShaderValueDeclaration(GLint index, GLint size, GLenum type, const char *name);

public:

    class findByName
    {
    public:
        findByName(const std::string &name) : _name(name) {}

        bool operator () (const ShaderValueDeclaration &svd) const
        {
            return svd._name == _name;
        }

    private:
        std::string _name;
    };

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

    inline operator bool ()
    {
        return _size != 0;
    }

    UniformBinder bind()
    {
        return UniformBinder(_index);
    }

    UniformBinder operator* ()
    {
        return UniformBinder(_index);
    }

private:
    void normalizeArrayName();
    GLint _index;
    GLint _size;
    GLenum _type;
    std::string _name;
};

class UniformDeclaration : public ShaderValueDeclaration
{
public:
    UniformDeclaration();
    UniformDeclaration(GLint index, GLint size, GLenum type, const char *name);
};

typedef std::vector<UniformDeclaration> UniformDeclarationVector;

class VertexAttributeDeclaration : public ShaderValueDeclaration
{
public:
    VertexAttributeDeclaration();
    VertexAttributeDeclaration(GLint index, GLint size, GLenum type, const char *name);
};

typedef std::vector<VertexAttributeDeclaration> VertexAttributeDeclarationVector;

}

#endif // ShaderValueDeclaration_H
