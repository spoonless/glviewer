#ifndef LOG_HPP
#define LOG_HPP

#include <memory>
#include "config.h"

#ifdef USE_G3LOG

#include "g3log/g3log.hpp"

#else

#include <cassert>
#include <ostream>

#define DEBUG 0
#define INFO 0
#define WARNING 0
#define FATAL 0

namespace sys
{

namespace ignore
{
    extern std::ostream nullostream;
}

}

#define LOG(level) if(false) sys::ignore::nullostream
#define LOGF(level, printf_like_message, ...) if(false) assert(1)
#define CHECK(x) assert(x)

#endif

namespace sys
{
    class LoggingSystem
    {
    public:
        LoggingSystem(const LoggingSystem &) = delete;
        LoggingSystem & operator = (const LoggingSystem &) = delete;
        virtual ~LoggingSystem();

        static std::unique_ptr<LoggingSystem> create();

    protected:
        LoggingSystem();
    };
}

#define INIT_LOGGING_SYSTEM() auto loggingSystem = sys::LoggingSystem::create(); ((void*) &loggingSystem)

#endif
