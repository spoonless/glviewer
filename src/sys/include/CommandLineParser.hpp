#ifndef COMMAND_LINE_PARSER_HPP
#define COMMAND_LINE_PARSER_HPP

#include <functional>
#include <string>
#include <vector>
#include <cstring>
#include <ostream>

#include "Argument.hpp"

namespace sys
{

class CommandLineOption
{
    friend class CommandLineParser;
public:

    CommandLineOption(BaseArgument *argument);
    CommandLineOption(CommandLineOption &&clp);
    CommandLineOption(const CommandLineOption &) = default;

    CommandLineOption & description(char const *value);
    CommandLineOption & name(char const *name);
    CommandLineOption & shortName(char const *value);

private:
    bool matches(const char *v);

    BaseArgument *_argument;
    std::string _description;
    std::string _shortName;
    std::string _name;
};

class CommandLineParameter
{
    friend class CommandLineParser;
public:

    CommandLineParameter(BaseArgument *argument);
    CommandLineParameter(CommandLineParameter &&cla);
    CommandLineParameter(const CommandLineParameter &) = default;

    CommandLineParameter & selector(std::function<bool(const char*)> selector);
    CommandLineParameter & pattern(const char *pattern);

    CommandLineParameter & description(char const *value);
    CommandLineParameter & placeholder(char const *value);

private:
    bool matches(const char *v);

    BaseArgument *_argument;
    std::function<bool(const char*)> _selector;
    std::string _placeholder;
    std::string _description;
};

class CommandLineParser
{
public:

    OperationResult parse(int argc, char const **argv);

    CommandLineOption &option(BaseArgument &arg);
    CommandLineParameter &parameter(BaseArgument &arg);
    void validator(std::function<OperationResult()> validator);

    void displayArguments(std::ostream &os) const;

private:
    std::vector<CommandLineOption> _commandLineOptions;
    std::vector<CommandLineParameter> _commandLineParameters;
    std::function<OperationResult()> _validator;
};

std::ostream & operator << (std::ostream &os, const CommandLineParser &clp);

}

#endif
