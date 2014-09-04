#ifndef PATH_H
#define PATH_H

namespace system
{

class Path
{
public:
    static const char SEPARATOR;

    Path(const char *path = 0);
    Path(const Path &path);
    Path(const Path &parent, const Path &path);
    ~Path();

    Path& operator = (const Path &path);
    operator const char*() const;

    inline bool absolute() const
    {
        return _absoluteSectionLength > 0;
    }

private:
    void normalize();
    void computeAbsoluteSectionLength();
    unsigned int _length;
    unsigned int _absoluteSectionLength;
    char *_path;
};

}

#endif
