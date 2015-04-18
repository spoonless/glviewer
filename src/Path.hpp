#ifndef PATH_H
#define PATH_H

#include <cstddef>

namespace sys
{

class Path
{
public:
    static const char SEPARATOR;

    Path(const char *path = 0, std::size_t size = 0);
    Path(Path &&path);
    Path(const Path &path);
    Path(const Path &parent, const Path &path);
    ~Path();

    Path& operator = (const Path &path);

    operator const char*() const;

    inline bool absolute() const
    {
        return _absoluteSectionLength > 0;
    }

    const char *basename() const;
    const char *extension() const;

    Path dirpath() const;
    Path withoutExtension() const;

private:
    void normalize();
    void computeAbsoluteSectionLength();
    std::size_t _length;
    std::size_t _absoluteSectionLength;
    char *_path;
};

}

#endif
