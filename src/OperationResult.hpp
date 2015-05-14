#ifndef OPERATION_RESULT_H
#define OPERATION_RESULT_H

#include <string>

namespace sys
{

class OperationResult {

public:

    static inline OperationResult succeeded(unsigned long duration = 0)
    {
        return OperationResult(true, duration);
    }

    static inline OperationResult succeeded(const char *message, unsigned long duration = 0)
    {
        return OperationResult(true, message, duration);
    }

    static inline OperationResult succeeded(const std::string &message, unsigned long duration = 0)
    {
        return OperationResult(true, message, duration);
    }

    static inline OperationResult succeeded(std::string &&message, unsigned long duration = 0)
    {
        return OperationResult(true, message, duration);
    }

    static inline OperationResult failed(const char *message, unsigned long duration = 0)
    {
        return OperationResult(false, message, duration);
    }

    static inline OperationResult failed(const std::string &message, unsigned long duration = 0)
    {
        return OperationResult(false, message, duration);
    }

    static inline OperationResult failed(std::string &&message, unsigned long duration = 0)
    {
        return OperationResult(false, message, duration);
    }

    static inline OperationResult test(bool test, const char *errorMessage, unsigned long duration = 0)
    {
        return test ? OperationResult(test, duration) : OperationResult(test, errorMessage, duration);
    }

    static inline OperationResult test(bool test, std::string &errorMessage, unsigned long duration = 0)
    {
        return test ? OperationResult(test, duration) : OperationResult(test, errorMessage, duration);
    }

    static inline OperationResult test(bool test, std::string &&errorMessage, unsigned long duration = 0)
    {
        return test ? OperationResult(test, duration) : OperationResult(test, errorMessage, duration);
    }

    OperationResult(OperationResult &&operationResult)
    {
        this->_ok = operationResult.ok();
        this->_message = std::move(operationResult.message());
        this->_duration = operationResult.duration();
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
    OperationResult(bool ok, const char *message, unsigned long duration = 0) : _ok{ok}, _duration{duration}, _message(message)
    {
    }

    OperationResult(bool ok, const std::string &message, unsigned long duration = 0) : _ok{ok}, _duration{duration}, _message(message)
    {
    }

	OperationResult(bool ok, std::string &&message, unsigned long duration = 0) : _ok{ok}, _duration{duration}, _message(message)
    {
    }

    OperationResult(bool ok, unsigned long duration = 0) : _ok{ok}, _duration{duration}
    {
    }

    bool _ok;
    unsigned long _duration;
    std::string _message;
};

}

#endif
