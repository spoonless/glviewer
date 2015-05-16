#include <cctype>
#include "LineReader.hpp"

namespace
{

const auto BUFFER_CHUNK_SIZE = 256 * sizeof(char);

const char *leftTrim(const char* l)
{
    const char *start = l;
    for(; std::isspace(*start) && *start != 0; ++start);
    return start;
}

const char *rightTrim(char* line, char commentStarter = '#')
{
    char *end = line;
    for(; *end != 0; ++end)
    {
        if (*end == commentStarter)
        {
            if (end == line || std::isspace(*(end-1)))
            {
                *end = 0;
                break;
            }
        }
        else if (*end == '\r')
        {
            *end = 0;
            break;
        }
    }

    for(; end != line && std::isspace(*(end-1)); --end);
    *end = 0;
    return line;
}

}

namespace sys
{

LineReader::LineReader(std::istream &is) : _is(is), _capacity(BUFFER_CHUNK_SIZE), _lineNumber(0)
{
    _line = static_cast<char*>(std::malloc(_capacity * sizeof(char)));
    *_line = 0;
}

LineReader::~LineReader()
{
    std::free(_line);
}

const char *LineReader::read()
{
    copyReadLine();
    checkStream();
    return leftTrim(rightTrim(_line));
}

void LineReader::copyReadLine()
{
    *_line = 0;
    _is.getline(_line, _capacity);
    std::streamsize nbRead = 0;
    while((_is.rdstate() & std::istream::failbit) && _is.gcount() > 0 && !_is.eof())
    {
        nbRead += _is.gcount();
        _is.clear();
        _line = static_cast<char*>(std::realloc(_line, (_capacity + BUFFER_CHUNK_SIZE) * sizeof(char)));
        _is.getline(_line + nbRead, BUFFER_CHUNK_SIZE);
        _capacity += BUFFER_CHUNK_SIZE;
    }
}

void LineReader::checkStream()
{
    if (_is.eof())
    {
        _is.clear();
        _is.setstate(std::ios::eofbit);
    }
    if(!_is.fail())
    {
        ++_lineNumber;
    }
}

}
