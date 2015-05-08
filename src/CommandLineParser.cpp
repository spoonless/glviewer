#include <cstring>

#include "CommandLineParser.hpp"

namespace
{

template<typename T>
sys::OperationResult convertToInteger(T& dest, char const *src)
{
    char* end = nullptr;
    long long int value = std::strtoll(src, &end, 10);
    if (*end != 0 || src == end)
    {
        return sys::OperationResult::failed("cannot convert to number");
    }
    if (value > static_cast<long long int>(std::numeric_limits<T>::max()))
    {
        return sys::OperationResult::failed("value too large");
    }
    if (value < static_cast<long long int>(std::numeric_limits<T>::lowest()))
    {
        return sys::OperationResult::failed("value too small");
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
bool EnumArgument<char const *>::areEqual(char const * const&v1, char const * const&v2)
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
OperationResult CharArg::convert(char& dest, char const *src)
{
    if (std::strlen(src) > 1)
    {
        return OperationResult::failed("Only one character expected!");
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
CharSeqArg::Argument():_value(nullptr)
{
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

CommandLineOption::CommandLineOption(BaseArgument *argument): _argument(argument)
{
}

CommandLineOption::CommandLineOption(CommandLineOption &&clp):
    _argument(clp._argument),
    _description(std::move(clp._description)),
    _shortName(std::move(clp._shortName)),
    _name(std::move(clp._name))
{
}

CommandLineArgument::CommandLineArgument(BaseArgument *argument): _argument(argument)
{
}

CommandLineArgument::CommandLineArgument(CommandLineArgument &&desc):
    _argument(desc._argument)
{
}

bool CommandLineParser::parse(int argc, char const **argv)
{
    std::for_each(std::begin(_commandLineOptions), std::end(_commandLineOptions), [](CommandLineOption& clo){
        clo._argument->reset();
    });

    std::for_each(std::begin(_commandLineArguments), std::end(_commandLineArguments), [](CommandLineArgument& cla){
        cla._argument->reset();
    });

    bool result = true;
    for(int i = 1; result && i < argc; ++i)
    {
        bool found = false;
        for (CommandLineOption& clo : _commandLineOptions)
        {
            bool matchName = argv[i][0] == '-' && argv[i][1] == '-' && clo._name == (argv[i]+2);
            bool matchShortName = ! matchName && argv[i][0] == '-' && clo._shortName == (argv[i]+1);
            if (matchName || matchShortName)
            {
                if (clo._argument->isSwitch())
                {
                    result = clo._argument->convert("");
                    found = result;
                }
                else if (i+1 < argc)
                {
                    result = clo._argument->convert(argv[++i]);
                    found = result;
                }
                else {
                    // TODO provide an error message
                    result = false;
                }
                break;
            }
        }
        if (result && !found)
        {
            for (CommandLineArgument& cla : _commandLineArguments)
            {
                if(!*cla._argument)
                {
                    result = cla._argument->convert(argv[i]);
                    found = true;
                    break;
                }
            }
        }
        if (!found)
        {
            // TODO provide an error message
            result = false;
        }
    }

    return result;
}

CommandLineOption &CommandLineParser::option(BaseArgument& arg)
{
    _commandLineOptions.push_back(CommandLineOption(&arg));
    return _commandLineOptions.back();
}

CommandLineArgument &CommandLineParser::argument(BaseArgument& arg)
{
    _commandLineArguments.push_back(CommandLineArgument(&arg));
    return _commandLineArguments.back();
}

}
