#ifndef DURATION_H
#define DURATION_H

#include <ostream>
#include <string>

namespace sys
{

class Duration
{
public:
    Duration();

    Duration(const Duration &duration);

    Duration & operator = (const Duration &duration);

    unsigned long elapsed() const;

    operator std::string () const;


private:
    unsigned long _start;
};

inline std::ostream& operator << (std::ostream &os, const Duration &d)
{
    os << static_cast<std::string>(d);
    return os;
}

}


#endif // DURATION_H
