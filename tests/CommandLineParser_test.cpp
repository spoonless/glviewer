#include <algorithm>
#include <initializer_list>
#include <vector>
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

    for(int i = 0; i < argc; ++i)
    {
        for (ParseableArgument* pa : _arguments)
        {
            if (argv[i][0] == '-' && argv[i][1] == '-' && pa->_name == (argv[i]+2))
            {
                pa->convert(nullptr);
            }
            else if (argv[i][0] == '-' && pa->_shortName == (argv[i]+1))
            {
                pa->convert(nullptr);
            }
        }
    }
    return true;
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
    sys::BoolArg boolArg;
    boolArg.shortName("b");

    sys::CommandLineParser clp;
    clp.add(boolArg);

    char const *argv[] = {"-b"};
    bool result = clp.parse(1, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(boolArg);
    ASSERT_TRUE(boolArg.value());
}

TEST(CommandLineParser, canParseBoolArgByName)
{
    sys::BoolArg boolArg;
    boolArg.name("b");

    sys::CommandLineParser clp;
    clp.add(boolArg);

    char const *argv[] = {"--b"};
    bool result = clp.parse(1, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(boolArg);
    ASSERT_TRUE(boolArg.value());
}
