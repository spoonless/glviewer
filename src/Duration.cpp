#include "Duration.hpp"

Duration::Duration()
    :_start(clock())
{

}

Duration::Duration(const Duration& duration)
    :_start(duration._start)
{

}

Duration& Duration::operator = (const Duration& duration)
{
    if (this != &duration)
    {
        _start = duration._start;
    }
    return *this;
}

unsigned int Duration::elapsed() const
{
    return (unsigned long) (clock() - _start) * 1000 / CLOCKS_PER_SEC;
}
