#include <iostream>
#include "log.hpp"

#ifdef USE_G3LOG

#include "g3log/logworker.hpp"
namespace
{

class SimpleLogger
{
public:

    void log(g3::LogMessageMover lgm)
    {
        g3::LogMessage &logMsg = lgm.get();
        if(logMsg._level.value == WARNING.value)
        {
            std::clog << " ! ";
        }
        else if(logMsg.wasFatal())
        {
            std::clog << "!! ";
        }
        else
        {
            std::clog << " * ";
        }

        std::clog << logMsg.write() << std::endl;
    }

};

class ClogLoggingSystem : public sys::LoggingSystem
{
public:
    ClogLoggingSystem()
    {
        _logworker = g3::LogWorker::createLogWorker();
        _logworker->addSink(std::unique_ptr<SimpleLogger>(new SimpleLogger), &SimpleLogger::log);

        g3::initializeLogging(_logworker.get());
    }

private:
    std::unique_ptr<g3::LogWorker> _logworker;
};

}

std::unique_ptr<sys::LoggingSystem> sys::LoggingSystem::create()
{
    return std::unique_ptr<sys::LoggingSystem>(new ClogLoggingSystem);
}

#else

std::unique_ptr<sys::LoggingSystem> sys::LoggingSystem::create()
{
    return std::unique_ptr<sys::LoggingSystem>(new LoggingSystem());
}

#endif

sys::LoggingSystem::LoggingSystem()
{
}

sys::LoggingSystem::~LoggingSystem()
{
}
