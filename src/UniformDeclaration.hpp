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
    friend class UniformDeclaration;
public:

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
    UniformBinder& operator = (const glm::f32mat2x4 &v);

    UniformBinder& operator = (const glm::f32mat3x2 &v);
    UniformBinder& operator = (const glm::f32mat3x4 &v);

    UniformBinder& operator = (const glm::f32mat4x2 &v);
    UniformBinder& operator = (const glm::f32mat4x3 &v);

    operator glm::f32() const;
    operator glm::fvec2() const;
    operator glm::fvec3() const;
    operator glm::fvec4() const;

    operator glm::i32() const;
    operator glm::ivec2() const;
    operator glm::ivec3() const;
    operator glm::ivec4() const;

    operator glm::u32() const;
    operator glm::uvec2() const;
    operator glm::uvec3() const;
    operator glm::uvec4() const;

    operator glm::f32mat2() const;
    operator glm::f32mat3() const;
    operator glm::f32mat4() const;

    operator glm::f32mat2x3() const;
    operator glm::f32mat2x4() const;

    operator glm::f32mat3x2() const;
    operator glm::f32mat3x4() const;

    operator glm::f32mat4x2() const;
    operator glm::f32mat4x3() const;

private:
    UniformBinder(GLuint programId = 0, GLint index = -1, GLint size = 0, GLenum type = GL_INT);
    UniformBinder(const UniformBinder &uniformBinder);
    bool operator == (const UniformBinder &uniformBinder) const;

    GLuint _programId;
    GLint _index;
    GLint _size;
    GLenum _type;
};

class UniformDeclaration
{
public:
    UniformDeclaration();
    UniformDeclaration(GLuint programId, GLint index, GLint size, GLenum type, const char *name);

    bool operator == (const UniformDeclaration &ud) const;

    inline bool operator != (const UniformDeclaration &ud) const
    {
        return !(this->operator == (ud));
    }

    inline GLuint getProgramId() const
    {
        return _binder._programId;
    }

    UniformBinder& operator* ()
    {
        return _binder;
    }

    const UniformBinder& operator* () const
    {
        return _binder;
    }

    inline const std::string& name() const
    {
        return _name;
    }

    inline GLuint index() const
    {
        return _binder._index;
    }

    inline GLint size() const
    {
        return _binder._size;
    }

    inline GLenum type() const
    {
        return _binder._type;
    }

    inline bool isArray() const
    {
        return _binder._size > 1;
    }

    inline operator bool () const
    {
        return _binder._size != 0;
    }

private:
    UniformBinder _binder;
    std::string _name;
};

typedef std::vector<UniformDeclaration> UniformDeclarationVector;

class VertexAttributeDeclaration
{
public:
    VertexAttributeDeclaration();
    VertexAttributeDeclaration(GLint index, GLint size, GLenum type, const char *name);

    bool operator == (const VertexAttributeDeclaration &vad) const;

    inline bool operator != (const VertexAttributeDeclaration &vad) const
    {
        return !(this->operator ==(vad));
    }

    inline const std::string& name() const
    {
        return _name;
    }

    inline GLuint index() const
    {
        return _index;
    }

    inline GLint size() const
    {
        return _size;
    }

    inline GLenum type() const
    {
        return _type;
    }

    inline bool isArray() const
    {
        return _size > 1;
    }

    inline operator bool () const
    {
        return _size != 0;
    }

private:
    GLint _index;
    GLint _size;
    GLenum _type;
    std::string _name;
};

typedef std::vector<VertexAttributeDeclaration> VertexAttributeDeclarationVector;

}

#endif // ShaderValueDeclaration_H
