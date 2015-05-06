#include <limits>
#include <algorithm>
#include <vector>
#include <cstdlib>

#include <gtest/gtest.h>

#include "OperationResult.hpp"

namespace sys
{

using OperationResult = gl::OperationResult;

class ParseableArgument
{
    friend class CommandLineParser;
public:
    ParseableArgument() : _valueSet(false)
    {
    }

    inline operator bool() const
    {
        return _valueSet;
    }

protected:
    virtual OperationResult convert(char const *value) = 0;

    virtual bool isSwitch() const = 0;

    virtual void reset() = 0;

    bool _valueSet;
    std::string _description;
    std::string _shortName;
    std::string _name;
    std::string _mappedFileExtension;
};

template<typename T>
class Argument : public ParseableArgument
{
public:
    using ValueType = T;

    Argument()
    {
        reset(_value);
    }

    Argument<T> & description(char const *value)
    {
        _description = value;
        return *this;
    }

    Argument<T> & name(char const *name)
    {
        _name = name;
        return *this;
    }

    Argument<T> & shortName(char const *value)
    {
        _shortName = value;
        return *this;
    }

    Argument<T> & mappedFileExtension(char const *value)
    {
        _mappedFileExtension = value;
        return *this;
    }

    inline operator bool() const
    {
        return _valueSet;
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

    bool isSwitch() const override;

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

template<typename T>
bool Argument<T>::isSwitch() const
{
    return false;
}

using BoolArg = Argument<bool>;

template<>
bool BoolArg::isSwitch() const
{
    return true;
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

class CommandLineParser
{
public:

    bool parse(int argc, char const **argv);

    void add(ParseableArgument& arg);

private:
    std::vector<ParseableArgument*> _arguments;

};


bool CommandLineParser::parse(int argc, char const **argv)
{
    std::for_each(_arguments.begin(), _arguments.end(), [](ParseableArgument* pa){
        pa->reset();
    });

    bool result = true;
    for(int i = 0; result && i < argc; ++i)
    {
        for (ParseableArgument* pa : _arguments)
        {
            bool matchName = argv[i][0] == '-' && argv[i][1] == '-' && pa->_name == (argv[i]+2);
            bool matchShortName = ! matchName && argv[i][0] == '-' && pa->_shortName == (argv[i]+1);
            if (matchName || matchShortName)
            {
                if (pa->isSwitch())
                {
                    result = pa->convert("");
                }
                else if (i+1 < argc)
                {
                    result = pa->convert(argv[++i]);
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

void CommandLineParser::add(ParseableArgument& arg)
{
    _arguments.push_back(&arg);
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
    arg.shortName("b");

    sys::CommandLineParser clp;
    clp.add(arg);

    char const *argv[] = {"-b"};
    bool result = clp.parse(1, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_TRUE(arg.value());
}

TEST(CommandLineParser, canParseBoolArgByName)
{
    sys::BoolArg arg;
    arg.name("b");

    sys::CommandLineParser clp;
    clp.add(arg);

    char const *argv[] = {"--b"};
    bool result = clp.parse(1, argv);

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
    arg.name("i");

    sys::CommandLineParser clp;
    clp.add(arg);

    char const *argv[] = {"--i", "1"};
    bool result = clp.parse(2, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_EQ(1, arg.value());
}

TEST(CommandLineParser, canParseIntArgWithEmptyValue)
{
    sys::IntArg arg;
    arg.name("i");

    sys::CommandLineParser clp;
    clp.add(arg);

    char const *argv[] = {"--i", "  "};
    bool result = clp.parse(2, argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
    ASSERT_EQ(0, arg.value());
}

TEST(CommandLineParser, cannotParseIntArgWhenNoValueIsProvided)
{
    sys::IntArg arg;
    arg.name("i");

    sys::CommandLineParser clp;
    clp.add(arg);

    char const *argv[] = {"--i"};
    bool result = clp.parse(1, argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
    ASSERT_EQ(0, arg.value());
}

TEST(CommandLineParser, cannotParseIntArg)
{
    sys::IntArg arg;
    arg.name("i");

    sys::CommandLineParser clp;
    clp.add(arg);

    char const *argv[] = {"--i", "12upaer"};
    bool result = clp.parse(2, argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
    ASSERT_EQ(0, arg.value());
}

TEST(CommandLineParser, canParseLongLongArgByName)
{
    sys::LongLongArg arg;
    arg.name("l");

    sys::CommandLineParser clp;
    clp.add(arg);

    char const *argv[] = {"--l", "-10000"};
    bool result = clp.parse(2, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_EQ(-10000ll, arg.value());
}

TEST(CommandLineParser, canParseLongArgByName)
{
    sys::LongArg arg;
    arg.name("l");

    sys::CommandLineParser clp;
    clp.add(arg);

    char const *argv[] = {"--l", "-1000"};
    bool result = clp.parse(2, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_EQ(-1000l, arg.value());
}

TEST(CommandLineParser, canParseShortArgByName)
{
    sys::ShortArg arg;
    arg.name("s");

    sys::CommandLineParser clp;
    clp.add(arg);

    char const *argv[] = {"--s", "10"};
    bool result = clp.parse(2, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_EQ(10, arg.value());
}

TEST(CommandLineParser, cannotParseShortArgWhenValueTooLarge)
{
    sys::ShortArg arg;
    arg.name("s");

    sys::CommandLineParser clp;
    clp.add(arg);

    char const *argv[] = {"--s", "70000"};
    bool result = clp.parse(2, argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
    ASSERT_EQ(0, arg.value());
}

TEST(CommandLineParser, cannotParseShortArgWhenValueTooSmall)
{
    sys::ShortArg arg;
    arg.name("s");

    sys::CommandLineParser clp;
    clp.add(arg);

    char const *argv[] = {"--s", "-70000"};
    bool result = clp.parse(2, argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
    ASSERT_EQ(0, arg.value());
}
