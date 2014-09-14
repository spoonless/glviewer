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

using namespace glv;


UniformBinder& UniformBinder::operator = (const glm::f32 &v)
{
    glUniform1f(_uniformDeclaration->getIndex(), v);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::fvec2 &v)
{
    glUniform2f(_uniformDeclaration->getIndex(), v.x, v.y);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::fvec3 &v)
{
    glUniform3f(_uniformDeclaration->getIndex(), v.x, v.y, v.z);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::fvec4 &v)
{
    glUniform4f(_uniformDeclaration->getIndex(), v.x, v.y, v.z, v.w);
    return *this;
}


UniformBinder& UniformBinder::operator = (const glm::i32 &v)
{
    glUniform1i(_uniformDeclaration->getIndex(), v);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::i32vec2 &v)
{
    glUniform2i(_uniformDeclaration->getIndex(), v.x, v.y);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::i32vec3 &v)
{
    glUniform3i(_uniformDeclaration->getIndex(), v.x, v.y, v.z);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::i32vec4 &v)
{
    glUniform4i(_uniformDeclaration->getIndex(), v.x, v.y, v.z, v.w);
    return *this;
}


UniformBinder& UniformBinder::operator = (const glm::u32 &v)
{
    glUniform1ui(_uniformDeclaration->getIndex(), v);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::u32vec2 &v)
{
    glUniform2ui(_uniformDeclaration->getIndex(), v.x, v.y);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::u32vec3 &v)
{
    glUniform3ui(_uniformDeclaration->getIndex(), v.x, v.y, v.z);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::u32vec4 &v)
{
    glUniform4ui(_uniformDeclaration->getIndex(), v.x, v.y, v.z, v.w);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::f32mat2 &v)
{
    glUniformMatrix2fv(_uniformDeclaration->getIndex(), 1, false, &v[0][0]);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::f32mat3 &v)
{
    glUniformMatrix3fv(_uniformDeclaration->getIndex(), 1, false, &v[0][0]);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::f32mat4 &v)
{
    glUniformMatrix4fv(_uniformDeclaration->getIndex(), 1, false, &v[0][0]);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::f32mat2x3 &v)
{
    glUniformMatrix2x3fv(_uniformDeclaration->getIndex(), 1, false, &v[0][0]);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::f32mat3x2 &v)
{
    glUniformMatrix3x2fv(_uniformDeclaration->getIndex(), 1, false, &v[0][0]);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::f32mat2x4 &v)
{
    glUniformMatrix2x4fv(_uniformDeclaration->getIndex(), 1, false, &v[0][0]);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::f32mat4x2 &v)
{
    glUniformMatrix4x2fv(_uniformDeclaration->getIndex(), 1, false, &v[0][0]);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::f32mat3x4 &v)
{
    glUniformMatrix3x4fv(_uniformDeclaration->getIndex(), 1, false, &v[0][0]);
    return *this;
}

UniformBinder& UniformBinder::operator = (const glm::f32mat4x3 &v)
{
    glUniformMatrix4x3fv(_uniformDeclaration->getIndex(), 1, false, &v[0][0]);
    return *this;
}

UniformBinder::operator glm::f32() const
{
    glm::f32 v = .0f;
    if (_uniformDeclaration->getType() == GL_FLOAT && _uniformDeclaration->getSize() == 1)
    {
        glGetUniformfv(_uniformDeclaration->getProgramId(), _uniformDeclaration->getIndex(), &v);
    }
    return v;
}

UniformBinder::operator glm::fvec2() const
{
    glm::fvec2 v;
    if (_uniformDeclaration->getType() == GL_FLOAT_VEC2 && _uniformDeclaration->getSize() == 1)
    {
        glGetUniformfv(_uniformDeclaration->getProgramId(), _uniformDeclaration->getIndex(), &v[0]);
    }
    return v;
}

UniformBinder::operator glm::fvec3() const
{
    glm::fvec3 v;
    if (_uniformDeclaration->getType() == GL_FLOAT_VEC3 && _uniformDeclaration->getSize() == 1)
    {
        glGetUniformfv(_uniformDeclaration->getProgramId(), _uniformDeclaration->getIndex(), &v[0]);
    }
    return v;
}

UniformBinder::operator glm::fvec4() const
{
    glm::fvec4 v;
    if (_uniformDeclaration->getType() == GL_FLOAT_VEC4 && _uniformDeclaration->getSize() == 1)
    {
        glGetUniformfv(_uniformDeclaration->getProgramId(), _uniformDeclaration->getIndex(), &v[0]);
    }
    return v;
}

#define ARRAY_NORMALIZATION_SUFFIX_LENGTH 3
#define ARRAY_NORMALIZATION_SUFFIX "[0]"

ShaderValueDeclaration::ShaderValueDeclaration() : _index(-1), _size(0), _type(GL_INT)
{
}

ShaderValueDeclaration::ShaderValueDeclaration(GLint index, GLint size, GLenum type, const char *name)
    :_index(index), _size(size), _type(type), _name(name)
{
    normalizeArrayName();
}

bool ShaderValueDeclaration::operator == (const ShaderValueDeclaration &uniformDeclaration) const
{
    return this->_index == uniformDeclaration._index
            && this->_size == uniformDeclaration._size
            && this->_type == uniformDeclaration._type
            && this->_name == uniformDeclaration._name;
}


void ShaderValueDeclaration::normalizeArrayName()
{
    /*
     * Uniform array name can be optionally suffixed by [0].
     * To remove any ambiguity based on OpenGL implementation,
     * Uniform array name is normalized by removing [0] if necessary.
     */
    if (_name.length() > ARRAY_NORMALIZATION_SUFFIX_LENGTH)
    {
        size_t position = _name.rfind(ARRAY_NORMALIZATION_SUFFIX);
        if (position == _name.length() - ARRAY_NORMALIZATION_SUFFIX_LENGTH)
        {
            _name.erase(position);
        }
    }
}

UniformDeclaration::UniformDeclaration()
{
}

UniformDeclaration::UniformDeclaration(GLuint programId, GLint index, GLint size, GLenum type, const char *name) : ShaderValueDeclaration(index, size, type, name), _programId(programId)
{
}

VertexAttributeDeclaration::VertexAttributeDeclaration()
{
}

VertexAttributeDeclaration::VertexAttributeDeclaration(GLint index, GLint size, GLenum type, const char *name) : ShaderValueDeclaration(index, size, type, name)
{
}

