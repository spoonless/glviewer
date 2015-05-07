#include <limits>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cstring>

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

class CommandLineArgument
{
    friend class CommandLineParser;
public:

    CommandLineArgument(BaseArgument *argument): _argument(argument)
    {
    }

    CommandLineArgument(CommandLineArgument &&desc):
        _argument(desc._argument),
        _description(std::move(desc._description)),
        _shortName(std::move(desc._shortName)),
        _name(std::move(desc._name))
    {
    }

    CommandLineArgument(const CommandLineArgument &desc) = delete;

    CommandLineArgument & description(char const *value)
    {
        _description = value;
        return *this;
    }

    CommandLineArgument & name(char const *name)
    {
        _name = name;
        return *this;
    }

    CommandLineArgument & shortName(char const *value)
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

    inline operator bool() const
    {
        return _valueSet;
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

private:

    static OperationResult convert(ValueType& dest, char const *src) = delete;
    static void reset(ValueType& dest) = delete;

    ValueType _value;

};

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

using StringArg = Argument<std::string>;

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

    CommandLineArgument &parameter(BaseArgument& arg);

private:
    std::vector<CommandLineArgument> _commandLineArguments;

};

bool CommandLineParser::parse(int argc, char const **argv)
{
    std::for_each(_commandLineArguments.begin(), _commandLineArguments.end(), [](CommandLineArgument& pa){
        pa._argument->reset();
    });

    bool result = true;
    for(int i = 1; result && i < argc; ++i)
    {
        for (CommandLineArgument& pa : _commandLineArguments)
        {
            bool matchName = argv[i][0] == '-' && argv[i][1] == '-' && pa._name == (argv[i]+2);
            bool matchShortName = ! matchName && argv[i][0] == '-' && pa._shortName == (argv[i]+1);
            if (matchName || matchShortName)
            {
                if (pa._argument->isSwitch())
                {
                    result = pa._argument->convert("");
                }
                else if (i+1 < argc)
                {
                    result = pa._argument->convert(argv[++i]);
                }
                else {
                    // TODO provide an error message
                    result = false;
                }
                break;
            }
        }
    }

    return result;
}

CommandLineArgument &CommandLineParser::parameter(BaseArgument& arg)
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
    clp.parameter(arg).shortName("b");

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
    clp.parameter(arg).name("b");

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
    clp.parameter(arg).name("i");

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
    clp.parameter(arg).name("i");

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
    clp.parameter(arg).name("i");

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
    clp.parameter(arg).name("i");

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
    clp.parameter(arg).name("l");

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
    clp.parameter(arg).name("l");

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
    clp.parameter(arg).name("s");

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
    clp.parameter(arg).name("s");

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
    clp.parameter(arg).name("s");

    char const *argv[] = {"", "--s", "-70000"};
    bool result = clp.parse(3, argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
    ASSERT_EQ(0, arg.value());
}

TEST(CommandLineParser, canParseStringArgByName)
{
    sys::StringArg arg;

    sys::CommandLineParser clp;
    clp.parameter(arg).name("string");

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
    clp.parameter(arg).name("string");

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
    clp.parameter(arg1).name("arg1");
    clp.parameter(arg2).name("arg2");
    clp.parameter(arg3).name("arg3");
    clp.parameter(arg4).name("arg4").shortName("4");

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
