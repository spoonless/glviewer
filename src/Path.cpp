#include <cstring>
#include <algorithm>
#include "Path.hpp"

#ifdef WIN32
const char sys::Path::SEPARATOR = '\\';
#define ALTERNATE_SEPARATOR '/'
#else
const char sys::Path::SEPARATOR = '/';
#define ALTERNATE_SEPARATOR '\\'
#endif

namespace
{
const char* EMPTY_PATH="";
}

sys::Path::Path(const char *path, size_t size): _length(0), _absoluteSectionLength(0), _path(nullptr)
{
    if(path != nullptr)
    {
        this->_length = size == 0 ? std::strlen(path) : size;
        if(this->_length > 0)
        {
            this->_path = new char[this->_length+1];
            std::memcpy(this->_path, path, (this->_length) * sizeof(char));
            this->_path[this->_length] = 0;
            this->normalize();
            this->computeAbsoluteSectionLength();
        }
    }
}

sys::Path::Path(Path &&path): _length(path._length), _absoluteSectionLength(path._absoluteSectionLength), _path(path._path)
{
    path._length = 0;
    path._absoluteSectionLength = 0;
    path._path = nullptr;
}

sys::Path::Path(const Path &path): _length(path._length), _absoluteSectionLength(path._absoluteSectionLength), _path(nullptr)
{
    if(this->_length > 0)
    {
        this->_path = new char[this->_length+1];
        std::memcpy(this->_path, path._path, (this->_length +1) * sizeof(char));
    }
}

sys::Path::Path(const Path &parent, const Path &path): _length(path._length), _absoluteSectionLength(0), _path(nullptr)
{
    if(path.absolute())
    {
        this->_absoluteSectionLength = path._absoluteSectionLength;
        this->_path = new char[this->_length+1];
        std::memcpy(this->_path, path._path, (path._length +1) * sizeof(char));
    }
    else
    {
        bool need_separator = parent._length > 0 && parent._path[parent._length - 1] != Path::SEPARATOR;
        this->_absoluteSectionLength = parent._absoluteSectionLength;
        this->_length += parent._length + need_separator;
        if (this->_length > 0) {
            this->_path = new char[this->_length + 1];
            if (parent._length > 0)
            {
                std::memcpy(this->_path, parent._path, parent._length * sizeof(char));
                if (need_separator)
                {
                    this->_path[parent._length] = Path::SEPARATOR;
                    this->_path[parent._length + 1] = 0;
                }
            }
            if (path._length > 0)
            {
                std::memcpy(this->_path + parent._length + need_separator, path._path, (path._length + 1) * sizeof(char));
            }
        }
    }
}

sys::Path::~Path()
{
    delete[] this->_path;
}

sys::Path& sys::Path::operator = (const Path &path)
{
    if (this == &path)
    {
        return *this;
    }

    if(this->_length < path._length)
    {
        delete[] this->_path;
        this->_path = new char[path._length+1];
    }
    this->_length = path._length;
    this->_absoluteSectionLength = path._absoluteSectionLength;
    if (this->_path)
    {
        this->_path[0] = 0;
        if(path._path)
        {
            std::memcpy(this->_path, path._path, (this->_length + 1) * sizeof(char));
        }
    }
    return *this;
}

sys::Path::operator const char*() const
{
    return this->_path != nullptr ? this->_path : EMPTY_PATH;
}

const char *sys::Path::basename() const
{
    if (this->_length == 0)
    {
        return EMPTY_PATH;
    }
    for (size_t i = this->_length; i > 0; --i)
    {
        if(this->_path[i-1] == Path::SEPARATOR)
        {
            return &this->_path[i];
        }
    }
    return this->_path;
}

const char *sys::Path::extension() const
{
    char *extension = 0;
	for (size_t i = this->_length; i > std::max(static_cast<size_t>(1), this->_absoluteSectionLength); --i)
    {
        if (this->_path[i-1] == '.')
        {
            if (this->_path[i-2] != Path::SEPARATOR)
            {
                extension = &this->_path[i];
            }
            break;
        }
        else if (this->_path[i-1] == Path::SEPARATOR)
        {
            break;
        }
    }
    return extension == 0 ? EMPTY_PATH : extension;
}

sys::Path sys::Path::withoutExtension() const
{
    const char *currentExtension = this->extension();
    if(currentExtension[0] == 0)
    {
        return *this;
    }
    else
    {
        return Path(this->_path, (currentExtension - 1) - this->_path);
    }
}

sys::Path sys::Path::dirpath() const
{
    size_t i = this->_length;
    for(; i > this->_absoluteSectionLength; --i)
    {
        if (this->_path[i-1] == Path::SEPARATOR && i < this->_length)
        {
            return Path(this->_path, i-1);
        }
    }
    return i == 0 ? Path(".") : Path(this->_path, i);
}

#ifdef WIN32

void sys::Path::computeAbsoluteSectionLength()
{
    this->_absoluteSectionLength = 0;
    if(this->_length > 0)
    {
        unsigned int pos = 0;
        if (this->_path[pos] == Path::SEPARATOR)
        {
            this->_absoluteSectionLength = 1;
            if (this->_path[++pos] == Path::SEPARATOR)
            {
                while(this->_path[++pos] != Path::SEPARATOR || pos == this->_length);
                this->_absoluteSectionLength = pos;
            }
        }
        else
        {
            while((this->_path[pos] >= 'a' && this->_path[pos] <= 'z') || (this->_path[pos] >= 'A' && this->_path[pos] <= 'Z'))
            {
                ++pos;
            }
            if (pos > 0 && this->_path[pos] == ':' && this->_path[pos+1] == Path::SEPARATOR)
            {
               this->_absoluteSectionLength = pos + 2;
            }
        }
    }
}

#else

void sys::Path::computeAbsoluteSectionLength()
{
    this->_absoluteSectionLength = this->_length > 0 && this->_path[0] == Path::SEPARATOR ? 1 : 0;
}

#endif

void sys::Path::normalize()
{
    if(this->_path != nullptr)
    {
        for(int i = 0; this->_path[i] != 0; ++i)
        {
            if (this->_path[i] == ALTERNATE_SEPARATOR)
            {
                this->_path[i] = Path::SEPARATOR;
            }
        }
    }
}
