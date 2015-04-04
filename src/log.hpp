#ifndef LOG_HPP
#define LOG_HPP

#include "config.h"

#ifdef USE_G3LOG

#include "g2log.hpp"

#else

#include <ostream>
#include <cassert>

#define DEBUG 0
#define INFO 0
#define WARNING 0
#define FATAL 0

namespace ignore
{
    extern std::ostream nullostream;
}

#define LOG(level) if(level) ignore::nullostream
#define LOGF(level, printf_like_message, ...) if(level) assert(1)
#define CHECK(x) assert(x)

#endif


namespace sys
{
    void initLogger();
}

#endif
