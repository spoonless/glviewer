#include <iostream>
#include "log.hpp"

#ifdef USE_G3LOG

#include "g2logworker.hpp"
namespace
{

class SimpleLogger
{
public:

    void log(g2::LogMessageMover lgm)
    {
        g2::LogMessage &logMsg = lgm.get();
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

std::unique_ptr<g2::LogWorker> logworker;

}

void sys::initLogger()
{
    logworker = g2::LogWorker::createWithNoSink();
    logworker->addSink(std::unique_ptr<SimpleLogger>{new SimpleLogger}, &SimpleLogger::log);

    g2::initializeLogging(logworker.get());
}

#else

void sys::initLogger()
{
}

#endif
