#include <cstring>
#include "Path.hpp"

#ifdef WIN32
const char system::Path::SEPARATOR = '\\';
#define ALTERNATE_SEPARATOR '/'
#else
const char system::Path::SEPARATOR = '/';
#define ALTERNATE_SEPARATOR '\\'
#endif

system::Path::Path(const char *path): _length(0), _absoluteSectionLength(0), _path(0)
{
    if(path != 0)
    {
        this->_length = std::strlen(path);
        if(this->_length > 0)
        {
            this->_path = new char[this->_length+1];
            std::memcpy(this->_path, path, (this->_length +1) * sizeof(char));
            this->normalize();
            this->computeAbsoluteSectionLength();
        }
    }
}

system::Path::Path(const Path &path): _length(path._length), _absoluteSectionLength(path._absoluteSectionLength), _path(0)
{
    if(this->_length > 0)
    {
        this->_path = new char[this->_length+1];
        std::memcpy(this->_path, path._path, (this->_length +1) * sizeof(char));
    }
}

system::Path::Path(const Path &parent, const Path &path): _length(path._length), _absoluteSectionLength(0), _path(0)
{
    if(path.absolute())
    {
        this->_absoluteSectionLength = path._absoluteSectionLength;
        this->_path = new char[this->_length+1];
        std::memcpy(this->_path, path._path, (path._length +1) * sizeof(char));
    }
    else
    {
        // TODO handle the case where parent path does not end with a separator
        this->_absoluteSectionLength = parent._absoluteSectionLength;
        this->_length += parent._length;
        this->_path = new char[this->_length+1];
        std::memcpy(this->_path, parent._path, parent._length * sizeof(char));
        std::memcpy(this->_path + parent._length, path._path, (path._length +1) * sizeof(char));
    }
}

system::Path::~Path()
{
    delete[] this->_path;
}

system::Path& system::Path::operator = (const Path &path)
{
    if(this->_length < path._length)
    {
        delete[] this->_path;
        this->_path = new char[path._length+1];
    }
    this->_length = path._length;
    this->_absoluteSectionLength = path._absoluteSectionLength;
    std::memcpy(this->_path, path._path, (this->_length +1) * sizeof(char));
    return *this;
}

system::Path::operator const char*() const
{
    return this->_path;
}

#ifdef WIN32

void system::Path::computeAbsoluteSectionLength()
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

void system::Path::computeAbsoluteSectionLength()
{
    this->_absoluteSectionLength = this->_length > 0 && this->_path[0] == Path::SEPARATOR ? 1 : 0;
}

#endif

void system::Path::normalize()
{
    if(this->_path != 0)
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
