#ifndef LINE_READER_HPP
#define LINE_READER_HPP

#include <cstddef>
#include <istream>

namespace sys
{

class LineReader
{
public:
    LineReader(std::istream &is);
    ~LineReader();

    LineReader(const LineReader &) = delete;
    LineReader &operator = (const LineReader &) = delete;

    const char *read();

    std::size_t lineNumber() const
    {
        return _lineNumber;
    }

    inline operator bool() const
    {
        return _is.good();
    }

private:
    void copyReadLine();
    void checkStream();

    std::istream &_is;
    std::size_t _capacity;
    std::size_t _lineNumber;
    char *_line;
};

}

#endif
