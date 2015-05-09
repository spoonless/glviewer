#ifndef COMMAND_LINE_PARSER_HPP
#define COMMAND_LINE_PARSER_HPP

#include <type_traits>
#include <algorithm>
#include <string>
#include <vector>
#include <cstring>

#include "OperationResult.hpp"

namespace sys
{

class BaseArgument
{
public:
    virtual ~BaseArgument();

    operator bool() const;

    bool operator !() const;

protected:
    friend class CommandLineParser;

    BaseArgument();

    virtual OperationResult convert(char const *value) = 0;
    virtual void reset() = 0;
    virtual bool isSwitch() const = 0;

    bool _valueSet;
};

inline BaseArgument::operator bool() const
{
    return _valueSet;
}

inline bool BaseArgument::operator !() const
{
    return !_valueSet;
}

template<typename T>
class Argument : public BaseArgument
{
public:
    using ValueType = T;

    Argument();
    ~Argument() override;

    T &value();
    const T &value() const;

protected:
    OperationResult convert(char const *value) override;

    bool isSwitch() const override;

    void reset() override;

protected:
    static OperationResult convert(ValueType& dest, char const *src);
    static void reset(ValueType& dest);

    ValueType _value;
};

template<typename T>
Argument<T>::Argument()
{
    reset(_value);
}

template<typename T>
Argument<T>::~Argument()
{
    reset(_value);
}

template<typename T>
inline T &Argument<T>::value()
{
    return _value;
}

template<typename T>
inline const T &Argument<T>::value() const
{
    return _value;
}

template<typename T>
OperationResult Argument<T>::convert(char const *value)
{
    OperationResult result = convert(_value, value);
    _valueSet = result.ok();
    return result;
}

template<typename T>
bool Argument<T>::isSwitch() const
{
    return std::is_same<T, bool>::value;
}

template<typename T>
void Argument<T>::reset()
{
    _valueSet = false;
    reset(_value);
}

template<typename T>
class EnumArgument : public Argument<T>
{
public:
    EnumArgument(std::initializer_list<T> l);

protected:
    OperationResult convert(char const *value) override;

private:
    static bool areEqual(const T &v1, const T  &v2);
    std::vector<T> _list;
};

template<typename T>
EnumArgument<T>::EnumArgument(std::initializer_list<T> l): _list(l)
{
}

template<typename T>
OperationResult EnumArgument<T>::convert(char const *value)
{
    OperationResult result = Argument<T>::convert(value);
    if (result)
    {
        auto found = std::find_if(std::begin(_list), std::end(_list), [this](const T& v){
            return areEqual(v, this->value());
        });

        if (found == std::end(_list))
        {
            Argument<T>::reset();
            return OperationResult::failed("not matched any of possible values");
        }
    }
    return result;
}

template<typename T>
bool EnumArgument<T>::areEqual(const T &v1, const T &v2)
{
    return v1 == v2;
}

class CommandLineOption
{
    friend class CommandLineParser;
public:

    CommandLineOption(BaseArgument *argument);
    CommandLineOption(CommandLineOption &&clp);
    CommandLineOption(const CommandLineOption &) = delete;

    CommandLineOption & description(char const *value);
    CommandLineOption & name(char const *name);
    CommandLineOption & shortName(char const *value);

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

    CommandLineArgument(BaseArgument *argument);

    CommandLineArgument(CommandLineArgument &&cla);

    CommandLineArgument(const CommandLineArgument &) = delete;

private:
    BaseArgument *_argument;
};

class CommandLineParser
{
public:

    bool parse(int argc, char const **argv);

    CommandLineOption &option(BaseArgument &arg);
    CommandLineArgument &argument(BaseArgument &arg);

private:
    std::vector<CommandLineOption> _commandLineOptions;
    std::vector<CommandLineArgument> _commandLineArguments;

};

using BoolArg = Argument<bool>;

using IntArg = Argument<int>;
using EnumIntArg = EnumArgument<int>;

using LongLongArg = Argument<long long int>;
using EnumLongLongArg = EnumArgument<long long int>;

using LongArg = Argument<long int>;
using EnumLongArg = EnumArgument<long int>;

using ShortArg = Argument<short int>;
using EnumShortArg = EnumArgument<short int>;

using UIntArg = Argument<unsigned int>;
using EnumUIntArg = EnumArgument<unsigned int>;

using ULongLongArg = Argument<unsigned long long int>;
using EnumULongLongArg = EnumArgument<unsigned long long int>;

using ULongArg = Argument<unsigned long int>;
using EnumULongArg = EnumArgument<unsigned long int>;

using UShortArg = Argument<unsigned short int>;
using EnumUShortArg = EnumArgument<unsigned short int>;

using CharArg = Argument<char>;
using EnumCharArg = EnumArgument<char>;

using StringArg = Argument<std::string>;
using EnumStringArg = EnumArgument<std::string>;

using CharSeqArg = Argument<const char*>;
using EnumCharSeqArg = EnumArgument<const char*>;

}

#endif
