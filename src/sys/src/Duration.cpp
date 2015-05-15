#include <chrono>
#include "Duration.hpp"

namespace
{

using clock = std::chrono::steady_clock;
using milliseconds = std::chrono::duration<unsigned long int, std::milli>;

auto beginningOfTime = clock::now();

inline unsigned long getDuration()
{
    return std::chrono::duration_cast<milliseconds>(clock::now() - beginningOfTime).count();
}

}

sys::Duration::Duration() : _start(getDuration())
{
}

sys::Duration::Duration(const Duration &duration)
    :_start(duration._start)
{

}

sys::Duration& sys::Duration::operator = (const Duration &duration)
{
    if (this != &duration)
    {
        _start = duration._start;
    }
    return *this;
}

unsigned long sys::Duration::elapsed() const
{
    return getDuration() - _start;
}
