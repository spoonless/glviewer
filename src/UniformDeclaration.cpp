#include "UniformDeclaration.hpp"

using namespace glv;

#define ARRAY_NORMALIZATION_SUFFIX_LENGTH 3
#define ARRAY_NORMALIZATION_SUFFIX "[0]"

UniformDeclaration::UniformDeclaration(GLuint index, GLint size, GLenum type, const char* name)
    :_index(index), _size(size), _type(type), _name(name)
{
    normalizeArrayName();
}

UniformDeclaration::UniformDeclaration(const UniformDeclaration& uniformDeclaration)
    :_index(uniformDeclaration._index), _size(uniformDeclaration._size), _type(uniformDeclaration._type), _name(uniformDeclaration._name)
{
}

UniformDeclaration& UniformDeclaration::operator = (const UniformDeclaration& uniformDeclaration)
{
    if (this != &uniformDeclaration)
    {
        _index = uniformDeclaration._index;
        _size = uniformDeclaration._size;
        _type = uniformDeclaration._type;
        _name = uniformDeclaration._name;
    }
    return *this;
}

bool UniformDeclaration::operator == (const UniformDeclaration& uniformDeclaration) const
{
    return this->_index == uniformDeclaration._index
            && this->_size == uniformDeclaration._size
            && this->_type == uniformDeclaration._type
            && this->_name == uniformDeclaration._name;
}


void UniformDeclaration::normalizeArrayName()
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
