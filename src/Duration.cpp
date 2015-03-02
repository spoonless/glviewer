#include <GLFW/glfw3.h>

#include "Duration.hpp"

namespace
{

inline unsigned long getTime()
{
    return static_cast<unsigned long>(glfwGetTime() * 1000);
}

}

sys::Duration::Duration()
    :_start{getTime()}
{

}

sys::Duration::Duration(const Duration &duration)
    :_start{duration._start}
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
    return static_cast<unsigned long>(getTime() - _start);
}
