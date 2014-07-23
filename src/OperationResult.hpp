#ifndef OPERATION_RESULT_H
#define OPERATION_RESULT_H

#include <string>

namespace glv
{

class OperationResult {

public:

    OperationResult(bool ok, const std::string &message, unsigned long duration = 0) : _ok(ok), _duration(duration), _message(message)
    {
    }

    operator bool() const
    {
        return _ok;
    }

    bool operator !() const
    {
        return !_ok;
    }

    inline bool ok() const
    {
        return _ok;
    }

    inline unsigned long duration() const
    {
        return _duration;
    }

    inline const std::string& message() const
    {
        return _message;
    }

private:
    bool _ok;
    unsigned long _duration;
    std::string _message;
};

}

#endif
