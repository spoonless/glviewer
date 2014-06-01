#ifndef DURATION_H
#define DURATION_H

#include <ctime>

class Duration
{
public:
    Duration();

    Duration(const Duration& duration);

    Duration& operator = (const Duration& duration);

    unsigned int elapsed() const;

private:
    clock_t _start;
};

#endif // DURATION_H
