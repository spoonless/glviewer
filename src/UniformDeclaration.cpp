#include "UniformDeclaration.hpp"

using namespace glv;

#define ARRAY_NORMALIZATION_SUFFIX_LENGTH 3
#define ARRAY_NORMALIZATION_SUFFIX "[0]"

ShaderValueDeclaration::ShaderValueDeclaration(GLuint index, GLint size, GLenum type, const char *name)
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

UniformDeclaration::UniformDeclaration(GLuint index, GLint size, GLenum type, const char *name) : ShaderValueDeclaration(index, size, type, name)
{
}

VertexAttributeDeclaration::VertexAttributeDeclaration(GLuint index, GLint size, GLenum type, const char *name) : ShaderValueDeclaration(index, size, type, name)
{
}

