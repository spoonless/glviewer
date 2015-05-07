#include <limits>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <functional>

#include <gtest/gtest.h>

#include "OperationResult.hpp"

namespace sys
{

using OperationResult = gl::OperationResult;

class BaseArgument
{
public:
    virtual ~BaseArgument()
    {
    }

    inline operator bool() const
    {
        return _valueSet;
    }

    inline bool operator !()
    {
        return !_valueSet;
    }

protected:
    friend class CommandLineParser;

    BaseArgument() : _valueSet(false)
    {
    }

    virtual OperationResult convert(char const *value) = 0;

    virtual void reset() = 0;

    virtual bool isSwitch() const = 0;

    bool _valueSet;
};

class CommandLineOption
{
    friend class CommandLineParser;
public:

    CommandLineOption(BaseArgument *argument): _argument(argument)
    {
    }

    CommandLineOption(CommandLineOption &&desc):
        _argument(desc._argument),
        _description(std::move(desc._description)),
        _shortName(std::move(desc._shortName)),
        _name(std::move(desc._name))
    {
    }

    CommandLineOption(const CommandLineOption &) = delete;

    CommandLineOption & description(char const *value)
    {
        _description = value;
        return *this;
    }

    CommandLineOption & name(char const *name)
    {
        _name = name;
        return *this;
    }

    CommandLineOption & shortName(char const *value)
    {
        _shortName = value;
        return *this;
    }

private:
    BaseArgument *_argument;
    std::string _description;
    std::string _shortName;
    std::string _name;
};

class CommandLineArgument
{
    friend class CommandLineParser;
public:

    CommandLineArgument(BaseArgument *argument): _argument(argument)
    {
    }

    CommandLineArgument(CommandLineArgument &&desc):
        _argument(desc._argument)
    {
    }

    CommandLineArgument(const CommandLineArgument &) = delete;

private:
    BaseArgument *_argument;
};

template<typename T>
class Argument : public BaseArgument
{
public:
    using ValueType = T;

    Argument()
    {
        reset(_value);
    }

    ~Argument() override
    {
        reset(_value);
    }

    inline T value() const
    {
        return _value;
    }

protected:
    OperationResult convert(char const *value) override
    {
        OperationResult result = convert(_value, value);
        _valueSet = result.ok();
        return result;
    }

    bool isSwitch() const override
    {
        return std::is_same<T, bool>::value;
    }

    void reset() override
    {
        _valueSet = false;
        reset(_value);
    }

protected:

    static OperationResult convert(ValueType& dest, char const *src) = delete;
    static void reset(ValueType& dest) = delete;

    ValueType _value;

};


template<typename T>
class EnumArgument : public Argument<T>
{
public:
    EnumArgument(std::initializer_list<T> l): _list(l)
    {

    }

protected:
    OperationResult convert(char const *value) override
    {
        OperationResult result = Argument<T>::convert(value);
        if (result)
        {
            auto found = std::find_if(std::begin(_list), std::end(_list), std::bind(areEqual, this->value(), std::placeholders::_1));

            if (found == std::end(_list))
            {
                Argument<T>::reset();
                return OperationResult::failed("not matched any of possible values");
            }
        }
        return result;
    }

private:

    static bool areEqual(const T &v1, const T  &v2);

    std::vector<T> _list;
};

template<typename T>
bool EnumArgument<T>::areEqual(const T &v1, const T &v2)
{
    return v1 == v2;
}

template<>
bool EnumArgument<char const *>::areEqual(char const * const&v1, char const * const&v2)
{
    return v1 == nullptr ? v1 == v2 : std::strcmp(v1, v2) == 0;
}

using BoolArg = Argument<bool>;

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

template<typename T>
OperationResult convertToInteger(T& dest, char const *src)
{
    char* end = nullptr;
    long long int value = std::strtoll(src, &end, 10);
    if (*end != 0 || src == end)
    {
        return OperationResult::failed("cannot convert to number");
    }
    if (value > static_cast<long long int>(std::numeric_limits<T>::max()))
    {
        return OperationResult::failed("value too large");
    }
    if (value < static_cast<long long int>(std::numeric_limits<T>::lowest()))
    {
        return OperationResult::failed("value too small");
    }
    dest = static_cast<T>(value);
    return OperationResult::succeeded();
}

using IntArg = Argument<int>;
using EnumIntArg = EnumArgument<int>;

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

using LongLongArg = Argument<long long int>;
using EnumLongLongArg = EnumArgument<long long int>;

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

using LongArg = Argument<long int>;
using EnumLongArg = EnumArgument<long int>;

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

using ShortArg = Argument<short int>;
using EnumShortArg = EnumArgument<short int>;

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

using CharArg = Argument<char>;
using EnumCharArg = EnumArgument<char>;

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

using StringArg = Argument<std::string>;
using EnumStringArg = EnumArgument<std::string>;

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

using CharSeqArg = Argument<const char*>;
using EnumCharSeqArg = EnumArgument<const char*>;

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

class CommandLineParser
{
public:

    bool parse(int argc, char const **argv);

    CommandLineOption &option(BaseArgument& arg);
    CommandLineArgument &argument(BaseArgument& arg);

private:
    std::vector<CommandLineOption> _commandLineOptions;
    std::vector<CommandLineArgument> _commandLineArguments;

};

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

TEST(CommandLineParser, boolArgIsNotParsedByDefault)
{
    sys::BoolArg boolArg;

    ASSERT_FALSE(boolArg);
    ASSERT_FALSE(boolArg.value());
}

TEST(CommandLineParser, canParseBoolArgByShortName)
{
    sys::BoolArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).shortName("b");

    char const *argv[] = {"", "-b"};
    bool result = clp.parse(2, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_TRUE(arg.value());
}

TEST(CommandLineParser, canParseBoolArgByName)
{
    sys::BoolArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).name("b");

    char const *argv[] = {"", "--b"};
    bool result = clp.parse(2, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_TRUE(arg.value());
}

TEST(CommandLineParser, intArgIsNotParsedByDefault)
{
    sys::IntArg arg;

    ASSERT_FALSE(arg);
    ASSERT_EQ(0, arg.value());
}

TEST(CommandLineParser, canParseIntArgByName)
{
    sys::IntArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).name("i");

    char const *argv[] = {"", "--i", "1"};
    bool result = clp.parse(3, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_EQ(1, arg.value());
}

TEST(CommandLineParser, canParseIntArgWithEmptyValue)
{
    sys::IntArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).name("i");

    char const *argv[] = {"", "--i", "  "};
    bool result = clp.parse(3, argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
    ASSERT_EQ(0, arg.value());
}

TEST(CommandLineParser, cannotParseIntArgWhenNoValueIsProvided)
{
    sys::IntArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).name("i");

    char const *argv[] = {"", "--i"};
    bool result = clp.parse(2, argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
    ASSERT_EQ(0, arg.value());
}

TEST(CommandLineParser, cannotParseIntArg)
{
    sys::IntArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).name("i");

    char const *argv[] = {"", "--i", "12upaer"};
    bool result = clp.parse(3, argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
    ASSERT_EQ(0, arg.value());
}

TEST(CommandLineParser, canParseLongLongArgByName)
{
    sys::LongLongArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).name("l");

    char const *argv[] = {"", "--l", "-10000"};
    bool result = clp.parse(3, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_EQ(-10000ll, arg.value());
}

TEST(CommandLineParser, canParseLongArgByName)
{
    sys::LongArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).name("l");

    char const *argv[] = {"", "--l", "-1000"};
    bool result = clp.parse(3, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_EQ(-1000l, arg.value());
}

TEST(CommandLineParser, canParseShortArgByName)
{
    sys::ShortArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).name("s");

    char const *argv[] = {"", "--s", "10"};
    bool result = clp.parse(3, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_EQ(10, arg.value());
}

TEST(CommandLineParser, cannotParseShortArgWhenValueTooLarge)
{
    sys::ShortArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).name("s");

    char const *argv[] = {"", "--s", "70000"};
    bool result = clp.parse(3, argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
    ASSERT_EQ(0, arg.value());
}

TEST(CommandLineParser, cannotParseShortArgWhenValueTooSmall)
{
    sys::ShortArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).name("s");

    char const *argv[] = {"", "--s", "-70000"};
    bool result = clp.parse(3, argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
    ASSERT_EQ(0, arg.value());
}

TEST(CommandLineParser, canParseCharArgByName)
{
    sys::CharArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).name("character");

    char const *argv[] = {"", "--character", "z"};
    bool result = clp.parse(3, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_EQ('z', arg.value());
}

TEST(CommandLineParser, cannotParseCharArgWhenMoreThanOneChar)
{
    sys::CharArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).name("character");

    char const *argv[] = {"", "--character", "zz"};
    bool result = clp.parse(3, argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
}

TEST(CommandLineParser, canParseStringArgByName)
{
    sys::StringArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).name("string");

    char const *argv[] = {"", "--string", "hello world"};
    bool result = clp.parse(3, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_EQ(std::string("hello world"), arg.value());
}

TEST(CommandLineParser, canParseCharSeqArgByName)
{
    sys::CharSeqArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).name("string");

    char const *argv[] = {"", "--string", "hello world"};
    bool result = clp.parse(3, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_STREQ("hello world", arg.value());
}

TEST(CommandLineParser, canParseMultipleArguments)
{
    sys::CharSeqArg arg1;
    sys::IntArg arg2;
    sys::BoolArg arg3;
    sys::BoolArg arg4;

    sys::CommandLineParser clp;
    clp.option(arg1).name("arg1");
    clp.option(arg2).name("arg2");
    clp.option(arg3).name("arg3");
    clp.option(arg4).name("arg4").shortName("4");

    char const *argv[] = {"", "--arg1", "hello", "-4", "--arg2", "10"};
    bool result = clp.parse(6, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg1);
    ASSERT_TRUE(arg2);
    ASSERT_FALSE(arg3);
    ASSERT_TRUE(arg4);

    ASSERT_STREQ("hello", arg1.value());
    ASSERT_EQ(10, arg2.value());
    ASSERT_TRUE(arg4.value());
}

TEST(CommandLineParser, canParseArguments)
{
    sys::CharSeqArg arg;

    sys::CommandLineParser clp;
    clp.argument(arg);

    char const *argv[] = {"", "hello"};
    bool result = clp.parse(2, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_STREQ("hello", arg.value());
}

TEST(CommandLineParser, cannotParseTooManyArguments)
{
    sys::CharSeqArg arg;

    sys::CommandLineParser clp;
    clp.argument(arg);

    char const *argv[] = {"", "hello", "world"};
    bool result = clp.parse(3, argv);

    ASSERT_FALSE(result);
}

TEST(CommandLineParser, canParseEnumeratedArguments)
{
    sys::EnumIntArg arg = {1,2,3};

    sys::CommandLineParser clp;
    clp.argument(arg);

    char const *argv[] = {"", "2"};
    bool result = clp.parse(2, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_EQ(2, arg.value());
}

TEST(CommandLineParser, canParseCharSeqEnumeratedArguments)
{
    sys::EnumCharSeqArg arg = {"a", "b", "c"};

    sys::CommandLineParser clp;
    clp.argument(arg);

    char const *argv[] = {"", "c"};
    bool result = clp.parse(2, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_STREQ("c", arg.value());
}

TEST(CommandLineParser, canParseStringEnumeratedArguments)
{
    sys::EnumStringArg arg = {"a", "b", "c"};

    sys::CommandLineParser clp;
    clp.argument(arg);

    char const *argv[] = {"", "a"};
    bool result = clp.parse(2, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_EQ(std::string("a"), arg.value());
}

TEST(CommandLineParser, cannotParseEnumeratedArguments)
{
    sys::EnumIntArg arg = {1,2,3};

    sys::CommandLineParser clp;
    clp.argument(arg);

    char const *argv[] = {"", "4"};
    bool result = clp.parse(2, argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
}
