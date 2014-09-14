#include "UniformDeclaration.hpp"

#include "glm/mat2x2.hpp"
#include "glm/mat2x3.hpp"
#include "glm/mat2x4.hpp"
#include "glm/mat3x2.hpp"
#include "glm/mat2x3.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat3x4.hpp"
#include "glm/mat4x2.hpp"
#include "glm/mat4x4.hpp"
#include "glm/mat4x3.hpp"

namespace
{

#define ARRAY_NORMALIZATION_SUFFIX_LENGTH 3
#define ARRAY_NORMALIZATION_SUFFIX "[0]"

void normalizeArrayName(std::string &name)
{
    /*
     * Uniform array name can be optionally suffixed by [0].
     * To remove any ambiguity based on OpenGL implementation,
     * Uniform array name is normalized by removing [0] if necessary.
     */
    if (name.length() > ARRAY_NORMALIZATION_SUFFIX_LENGTH)
    {
        size_t position = name.rfind(ARRAY_NORMALIZATION_SUFFIX);
        if (position == name.length() - ARRAY_NORMALIZATION_SUFFIX_LENGTH)
        {
            name.erase(position);
        }
    }
}

}

glv::UniformBinder::UniformBinder(GLuint programId, GLint index, GLint size, GLenum type) : _programId(programId), _index(index), _size(size), _type(type)
{
}

glv::UniformBinder::UniformBinder(const glv::UniformBinder &binder) : _programId(binder._programId), _index(binder._index), _size(binder._size), _type(binder._type)
{
}

bool glv::UniformBinder::operator == (const glv::UniformBinder &binder) const
{
    return _programId == binder._programId && _index == binder._index && _size == binder._size && _type == binder._type;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::f32 &v)
{
    glUniform1f(_index, v);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::fvec2 &v)
{
    glUniform2f(_index, v.x, v.y);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::fvec3 &v)
{
    glUniform3f(_index, v.x, v.y, v.z);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::fvec4 &v)
{
    glUniform4f(_index, v.x, v.y, v.z, v.w);
    return *this;
}


glv::UniformBinder& glv::UniformBinder::operator = (const glm::i32 &v)
{
    glUniform1i(_index, v);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::i32vec2 &v)
{
    glUniform2i(_index, v.x, v.y);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::i32vec3 &v)
{
    glUniform3i(_index, v.x, v.y, v.z);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::i32vec4 &v)
{
    glUniform4i(_index, v.x, v.y, v.z, v.w);
    return *this;
}


glv::UniformBinder& glv::UniformBinder::operator = (const glm::u32 &v)
{
    glUniform1ui(_index, v);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::u32vec2 &v)
{
    glUniform2ui(_index, v.x, v.y);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::u32vec3 &v)
{
    glUniform3ui(_index, v.x, v.y, v.z);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::u32vec4 &v)
{
    glUniform4ui(_index, v.x, v.y, v.z, v.w);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::f32mat2 &v)
{
    glUniformMatrix2fv(_index, 1, false, &v[0][0]);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::f32mat3 &v)
{
    glUniformMatrix3fv(_index, 1, false, &v[0][0]);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::f32mat4 &v)
{
    glUniformMatrix4fv(_index, 1, false, &v[0][0]);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::f32mat2x3 &v)
{
    glUniformMatrix2x3fv(_index, 1, false, &v[0][0]);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::f32mat3x2 &v)
{
    glUniformMatrix3x2fv(_index, 1, false, &v[0][0]);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::f32mat2x4 &v)
{
    glUniformMatrix2x4fv(_index, 1, false, &v[0][0]);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::f32mat4x2 &v)
{
    glUniformMatrix4x2fv(_index, 1, false, &v[0][0]);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::f32mat3x4 &v)
{
    glUniformMatrix3x4fv(_index, 1, false, &v[0][0]);
    return *this;
}

glv::UniformBinder& glv::UniformBinder::operator = (const glm::f32mat4x3 &v)
{
    glUniformMatrix4x3fv(_index, 1, false, &v[0][0]);
    return *this;
}

glv::UniformBinder::operator glm::f32() const
{
    glm::f32 v = .0f;
    if (_type == GL_FLOAT && _size == 1)
    {
        glGetUniformfv(_programId, _index, &v);
    }
    return v;
}

glv::UniformBinder::operator glm::fvec2() const
{
    glm::fvec2 v;
    if (_type == GL_FLOAT_VEC2 && _size == 1)
    {
        glGetUniformfv(_programId, _index, &v[0]);
    }
    return v;
}

glv::UniformBinder::operator glm::fvec3() const
{
    glm::fvec3 v;
    if (_type == GL_FLOAT_VEC3 && _size == 1)
    {
        glGetUniformfv(_programId, _index, &v[0]);
    }
    return v;
}

glv::UniformBinder::operator glm::fvec4() const
{
    glm::fvec4 v;
    if (_type == GL_FLOAT_VEC4 && _size == 1)
    {
        glGetUniformfv(_programId, _index, &v[0]);
    }
    return v;
}

//##############################################################

glv::UniformBinder::operator glm::i32() const
{
    glm::i32 v = 0;
    if (_type == GL_INT && _size == 1)
    {
        glGetUniformiv(_programId, _index, &v);
    }
    return v;
}

glv::UniformBinder::operator glm::ivec2() const
{
    glm::ivec2 v;
    if (_type == GL_INT_VEC2 && _size == 1)
    {
        glGetUniformiv(_programId, _index, &v[0]);
    }
    return v;
}

glv::UniformBinder::operator glm::ivec3() const
{
    glm::ivec3 v;
    if (_type == GL_INT_VEC3 && _size == 1)
    {
        glGetUniformiv(_programId, _index, &v[0]);
    }
    return v;
}

glv::UniformBinder::operator glm::ivec4() const
{
    glm::ivec4 v;
    if (_type == GL_INT_VEC4 && _size == 1)
    {
        glGetUniformiv(_programId, _index, &v[0]);
    }
    return v;
}

//##############################################################

glv::UniformBinder::operator glm::u32() const
{
    glm::u32 v = 0;
    if (_type == GL_UNSIGNED_INT && _size == 1)
    {
        glGetUniformuiv(_programId, _index, &v);
    }
    return v;
}

glv::UniformBinder::operator glm::uvec2() const
{
    glm::uvec2 v;
    if (_type == GL_UNSIGNED_INT_VEC2 && _size == 1)
    {
        glGetUniformuiv(_programId, _index, &v[0]);
    }
    return v;
}

glv::UniformBinder::operator glm::uvec3() const
{
    glm::uvec3 v;
    if (_type == GL_UNSIGNED_INT_VEC3 && _size == 1)
    {
        glGetUniformuiv(_programId, _index, &v[0]);
    }
    return v;
}

glv::UniformBinder::operator glm::uvec4() const
{
    glm::uvec4 v;
    if (_type == GL_UNSIGNED_INT_VEC4 && _size == 1)
    {
        glGetUniformuiv(_programId, _index, &v[0]);
    }
    return v;
}

//##############################################################

glv::UniformBinder::operator glm::f32mat2() const
{
    glm::f32mat2 v;
    if (_type == GL_FLOAT_MAT2 && _size == 1)
    {
        glGetUniformfv(_programId, _index, &v[0][0]);
    }
    return v;
}

glv::UniformBinder::operator glm::f32mat3() const
{
    glm::f32mat3 v;
    if (_type == GL_FLOAT_MAT3 && _size == 1)
    {
        glGetUniformfv(_programId, _index, &v[0][0]);
    }
    return v;
}

glv::UniformBinder::operator glm::f32mat4() const
{
    glm::f32mat4 v;
    if (_type == GL_FLOAT_MAT4 && _size == 1)
    {
        glGetUniformfv(_programId, _index, &v[0][0]);
    }
    return v;
}

//##############################################################

glv::UniformBinder::operator glm::f32mat2x3() const
{
    glm::f32mat2x3 v;
    if (_type == GL_FLOAT_MAT2x3 && _size == 1)
    {
        glGetUniformfv(_programId, _index, &v[0][0]);
    }
    return v;
}

glv::UniformBinder::operator glm::f32mat2x4() const
{
    glm::f32mat2x4 v;
    if (_type == GL_FLOAT_MAT2x4 && _size == 1)
    {
        glGetUniformfv(_programId, _index, &v[0][0]);
    }
    return v;
}

//##############################################################

glv::UniformBinder::operator glm::f32mat3x2() const
{
    glm::f32mat3x2 v;
    if (_type == GL_FLOAT_MAT3x2 && _size == 1)
    {
        glGetUniformfv(_programId, _index, &v[0][0]);
    }
    return v;
}

glv::UniformBinder::operator glm::f32mat3x4() const
{
    glm::f32mat3x4 v;
    if (_type == GL_FLOAT_MAT3x4 && _size == 1)
    {
        glGetUniformfv(_programId, _index, &v[0][0]);
    }
    return v;
}

//##############################################################

glv::UniformBinder::operator glm::f32mat4x2() const
{
    glm::f32mat4x2 v;
    if (_type == GL_FLOAT_MAT4x2 && _size == 1)
    {
        glGetUniformfv(_programId, _index, &v[0][0]);
    }
    return v;
}

glv::UniformBinder::operator glm::f32mat4x3() const
{
    glm::f32mat4x3 v;
    if (_type == GL_FLOAT_MAT4x3 && _size == 1)
    {
        glGetUniformfv(_programId, _index, &v[0][0]);
    }
    return v;
}

//##############################################################

glv::UniformDeclaration::UniformDeclaration()
{
}

glv::UniformDeclaration::UniformDeclaration(GLuint programId, GLint index, GLint size, GLenum type, const char *name) : _binder(programId, index, size, type), _name(name)
{
    normalizeArrayName(_name);
}

bool glv::UniformDeclaration::operator == (const glv::UniformDeclaration &ud) const
{
    return this == &ud || this->_binder == ud._binder;
}


glv::VertexAttributeDeclaration::VertexAttributeDeclaration() : _index(-1), _size(0), _type(GL_INT)
{
}

glv::VertexAttributeDeclaration::VertexAttributeDeclaration(GLint index, GLint size, GLenum type, const char *name) :_index(index), _size(size), _type(type), _name(name)
{
    normalizeArrayName(_name);
}

bool glv::VertexAttributeDeclaration::operator == (const glv::VertexAttributeDeclaration &vad) const
{
    return (this == &vad) ||
           (this->_index == vad._index
            && this->_size == vad._size
            && this->_type == vad._type
            && this->_name == vad._name);
}
