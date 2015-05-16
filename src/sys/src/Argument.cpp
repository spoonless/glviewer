#include <cstring>
#include <cerrno>
#include "Argument.hpp"

namespace
{

template<typename T, typename C = typename std::conditional<std::is_signed<T>::value, long long, unsigned long long>::type>
sys::OperationResult convertToInteger(T& dest, char const *src)
{
    char* end = nullptr;
    C value = 0;
    errno = 0;
    if (std::is_signed<T>::value)
    {
        value = std::strtoll(src, &end, 10);
    }
    else
    {
        value = std::strtoull(src, &end, 10);
    }
    if (errno == ERANGE)
    {
        errno = 0;
        return sys::OperationResult::failed("cannot convert to number!");
    }
    if (*end != 0 || src == end)
    {
        return sys::OperationResult::failed("cannot convert to number!");
    }
    if (value > static_cast<C>(std::numeric_limits<T>::max()))
    {
        return sys::OperationResult::failed("value too large!");
    }
    if (value < static_cast<C>(std::numeric_limits<T>::lowest()))
    {
        return sys::OperationResult::failed("value too small!");
    }
    dest = static_cast<T>(value);
    return sys::OperationResult::succeeded();
}

template<typename T>
sys::OperationResult convertToFloatingPointNumber(T& dest, char const *src)
{
    char* end = nullptr;
    errno = 0;
    long double value = std::strtold(src, &end);
    if (errno == ERANGE)
    {
        errno = 0;
        return sys::OperationResult::failed("cannot convert to floating point number!");
    }
    if (*end != 0 || src == end)
    {
        return sys::OperationResult::failed("cannot convert to floating point number!");
    }
    if (value > static_cast<long double>(std::numeric_limits<T>::max()))
    {
        return sys::OperationResult::failed("value too large!");
    }
    if (value < static_cast<long double>(std::numeric_limits<T>::lowest()))
    {
        return sys::OperationResult::failed("value too small!");
    }
    dest = static_cast<T>(value);
    return sys::OperationResult::succeeded();
}

}

namespace sys
{

BaseArgument::~BaseArgument()
{
}

BaseArgument::BaseArgument() : _valueSet(false)
{
}

template<>
bool EnumCharSeqArg::areEqual(char const * const&v1, char const * const&v2)
{
    return v1 == nullptr || v2 == nullptr ? v1 == v2 : std::strcmp(v1, v2) == 0;
}

template<>
OperationResult BoolArg::convert(bool& dest, char const *src)
{
    dest = true;
    return OperationResult::succeeded();
}

template<>
void BoolArg::reset(bool& value)
{
    value = false;
}

template<>
OperationResult IntArg::convert(int& dest, char const *src)
{
    return convertToInteger(dest, src);
}

template<>
void IntArg::reset(int& value)
{
    value = 0;
}

template<>
OperationResult LongLongArg::convert(long long int& dest, char const *src)
{
    return convertToInteger(dest, src);
}

template<>
void LongLongArg::reset(long long int& value)
{
    value = 0;
}

template<>
OperationResult LongArg::convert(long int& dest, char const *src)
{
    return convertToInteger(dest, src);
}

template<>
void LongArg::reset(long int& value)
{
    value = 0;
}

template<>
OperationResult ShortArg::convert(short int& dest, char const *src)
{
    return convertToInteger(dest, src);
}

template<>
void ShortArg::reset(short int& value)
{
    value = 0;
}

template<>
OperationResult UIntArg::convert(unsigned int& dest, char const *src)
{
    return convertToInteger(dest, src);
}

template<>
void UIntArg::reset(unsigned int& value)
{
    value = 0u;
}

template<>
OperationResult ULongLongArg::convert(unsigned long long int& dest, char const *src)
{
    return convertToInteger(dest, src);
}

template<>
void ULongLongArg::reset(unsigned long long int& value)
{
    value = 0u;
}

template<>
OperationResult ULongArg::convert(unsigned long int& dest, char const *src)
{
    return convertToInteger(dest, src);
}

template<>
void ULongArg::reset(unsigned long int& value)
{
    value = 0u;
}

template<>
OperationResult UShortArg::convert(unsigned short int& dest, char const *src)
{
    return convertToInteger(dest, src);
}

template<>
void UShortArg::reset(unsigned short int& value)
{
    value = 0u;
}

template<>
OperationResult CharArg::convert(char& dest, char const *src)
{
    if (std::strlen(src) > 1)
    {
        return OperationResult::failed("only one character expected!");
    }
    dest = src[0];
    return OperationResult::succeeded();
}

template<>
void CharArg::reset(char& value)
{
    value = 0;
}

template<>
OperationResult StringArg::convert(std::string& dest, char const *src)
{
    dest = src;
    return OperationResult::succeeded();
}

template<>
void StringArg::reset(std::string& value)
{
    value.clear();
}

template<>
CharSeqArg::Argument()
{
    _value = nullptr;
}

template<>
OperationResult CharSeqArg::convert(const char *& dest, char const *src)
{
    delete[] dest;
    dest = nullptr;
    if (src)
    {
        std::size_t s = std::strlen(src) + 1;
        dest = const_cast<const char*>(new char[s]);
        std::memcpy(const_cast<char*>(dest), src, s);
    }
    return OperationResult::succeeded();
}

template<>
void CharSeqArg::reset(const char *& value)
{
    delete[] value;
    value = nullptr;
}

template<>
OperationResult FloatArg::convert(float& dest, char const *src)
{
    return convertToFloatingPointNumber(dest, src);
}

template<>
void FloatArg::reset(float& value)
{
    value = 0;
}

template<>
OperationResult DoubleArg::convert(double& dest, char const *src)
{
    return convertToFloatingPointNumber(dest, src);
}

template<>
void DoubleArg::reset(double& value)
{
    value = 0;
}

template<>
OperationResult LongDoubleArg::convert(long double& dest, char const *src)
{
    return convertToFloatingPointNumber(dest, src);
}

template<>
void LongDoubleArg::reset(long double& value)
{
    value = 0;
}

}
