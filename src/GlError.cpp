#include "GlError.hpp"

using namespace glv;

GlError::GlError()
    :_errorFlag{GL_NO_ERROR}
{
    reset();
}

bool GlError::hasOccured()
{
    _errorFlag = glGetError();
    reset();
    return _errorFlag != GL_NO_ERROR;
}

std::string GlError::toString(const char* context) const
{
    static const char* defaultErrorMessage [] = {
        "No error detected.",
        "An unacceptable value is specified for an enumerated argument.",
        "A numeric argument is out of range.",
        "The specified operation is not allowed in the current state.",
        "The framebuffer object is not complete.",
        "There is not enough memory left to execute the command.",
        "An attempt has been made to perform an operation that would cause an internal stack to underflow.",
        "An attempt has been made to perform an operation that would cause an internal stack to overflow.",
        "An undefined error has occured"
    };

    std::string message = context;

    message += ": ";
    switch(_errorFlag)
    {
    case GL_NO_ERROR:
        message += defaultErrorMessage[0];
        break;
    case GL_INVALID_ENUM:
        message += defaultErrorMessage[1];
        break;
    case GL_INVALID_VALUE:
        message += defaultErrorMessage[2];
        break;
    case GL_INVALID_OPERATION:
        message += defaultErrorMessage[3];
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        message += defaultErrorMessage[4];
        break;
    case GL_OUT_OF_MEMORY:
        message += defaultErrorMessage[5];
        break;
    case GL_STACK_UNDERFLOW:
        message += defaultErrorMessage[6];
        break;
    case GL_STACK_OVERFLOW:
        message += defaultErrorMessage[7];
        break;
    default:
        message += defaultErrorMessage[8];
        break;
    }
    return message;
}

void GlError::reset()
{
    GLenum error = GL_NO_ERROR;
    do {
        error = glGetError();
    }
    while (error != GL_NO_ERROR);
}
