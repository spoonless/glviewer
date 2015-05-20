#include <regex>
#include <sstream>

#include "CommandLineParser.hpp"

namespace
{

bool isShortOption(const char *v, const std::string &optionName)
{
    return !optionName.empty() && v[0] == '-' && optionName == &v[1];
}

bool isLongOption(const char *v, const std::string &optionName)
{
    return !optionName.empty() && v[0] == '-' && v[1] == '-' && optionName == &v[2];
}

}

namespace sys
{

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

CommandLineOption & CommandLineOption::description(char const *value)
{
    _description = value;
    return *this;
}

CommandLineOption & CommandLineOption::name(char const *name)
{
    _name = name;
    return *this;
}

CommandLineOption & CommandLineOption::shortName(char const *value)
{
    _shortName = value;
    return *this;
}

bool CommandLineOption::matches(char const *value)
{
    return isShortOption(value, this->_shortName) || isLongOption(value, this->_name);
}

CommandLineParameter::CommandLineParameter(BaseArgument *argument): _argument(argument)
{
}

CommandLineParameter::CommandLineParameter(CommandLineParameter &&cla):
    _argument(cla._argument),
    _placeholder(std::move(cla._placeholder)),
    _description(std::move(cla._description)),
    _selector(std::move(cla._selector))
{
}

CommandLineParameter & CommandLineParameter::selector(std::function<bool(const char*)> selector)
{
    _selector = selector;
    return *this;
}

CommandLineParameter & CommandLineParameter::pattern(const char *pattern)
{
    std::regex regex(pattern);
    _selector = [regex](const char *v)
    {
        return std::regex_match(v, regex);
    };
    return *this;
}

CommandLineParameter & CommandLineParameter::description(char const *value)
{
    _description = value;
    return *this;
}

CommandLineParameter & CommandLineParameter::placeholder(char const *value)
{
    _placeholder = value;
    return *this;
}

bool CommandLineParameter::matches(char const *value)
{
    return !*_argument && (!_selector || _selector(value));
}

OperationResult CommandLineParser::parse(int argc, char const **argv)
{
    std::for_each(std::begin(_commandLineOptions), std::end(_commandLineOptions), [](CommandLineOption& clo){
        clo._argument->reset();
    });

    std::for_each(std::begin(_commandLineParameters), std::end(_commandLineParameters), [](CommandLineParameter& cla){
        cla._argument->reset();
    });

    std::stringstream message;
    for(int i = 1; i < argc; ++i)
    {
        bool found = false;
        for (CommandLineOption& clo : _commandLineOptions)
        {
            if (clo.matches(argv[i]))
            {
                if (clo._argument->isSwitch())
                {
                    OperationResult result = clo._argument->convert("true");
                    if (!result)
                    {
                        message << "Invalid option " << argv[i] << ": " << result.message();
                        return OperationResult::failed(message.str());
                    }
                }
                else if (i+1 < argc)
                {
                    OperationResult result = clo._argument->convert(argv[++i]);
                    if (!result)
                    {
                        message << "Invalid option " << argv[i-1] << " value: " << result.message();
                        return OperationResult::failed(message.str());
                    }
                }
                else {
                    message << "Invalid option " << argv[i] << " value: one value is expected!";
                    return OperationResult::failed(message.str());
                }
                found = true;
                break;
            }
        }

        if (!found)
        {
            for (CommandLineParameter& clp : _commandLineParameters)
            {
                if (clp.matches(argv[i]))
                {
                    OperationResult result = clp._argument->convert(argv[i]);
                    if (!result)
                    {
                        message << "Invalid parameter '" << argv[i] << "': " << result.message();
                        return OperationResult::failed(message.str());
                    }
                    found = true;
                    break;
                }
            }
        }

        if(!found)
        {
            message << "Unexpected parameter or option '" << argv[i] << "'!";
            return OperationResult::failed(message.str());
        }
    }

    return _validator ? _validator() : OperationResult::succeeded();
}

CommandLineOption &CommandLineParser::option(BaseArgument &arg)
{
    _commandLineOptions.push_back(CommandLineOption(&arg));
    return _commandLineOptions.back();
}

CommandLineParameter &CommandLineParser::parameter(BaseArgument &arg)
{
    _commandLineParameters.push_back(CommandLineParameter(&arg));
    return _commandLineParameters.back();
}

void CommandLineParser::validator(std::function<OperationResult()> validator)
{
    _validator = validator;
}

void CommandLineParser::displayArguments(std::ostream &os) const
{
    if (!_commandLineOptions.empty())
    {
        os << std::endl << "Options:" << std::endl;
    }
    for (const CommandLineOption& clo : _commandLineOptions)
    {
        if (!clo._name.empty() && !clo._shortName.empty())
        {
            os << "   -" << clo._shortName << ", --" << clo._name << std::endl;
        }
        else if (!clo._name.empty())
        {
            os << "   --" << clo._name << std::endl;
        }
        else if (!clo._shortName.empty())
        {
            os << "   -" << clo._shortName << std::endl;
        }
        if (!clo._description.empty() && (!clo._name.empty() || !clo._shortName.empty()))
        {
            os << "      " << clo._description << std::endl;
        }
    }

    if (!_commandLineParameters.empty())
    {
        os << std::endl << "Parameters:" << std::endl;
    }
    for (const CommandLineParameter& clp : _commandLineParameters)
    {
        if (!clp._placeholder.empty())
        {
            os << "   " << clp._placeholder << std::endl;
        }
        if (!clp._description.empty())
        {
            os << (clp._placeholder.empty() ? "   " : "      ") << clp._description << std::endl;
        }
    }
}

std::ostream & operator << (std::ostream &os, const CommandLineParser &clp)
{
    clp.displayArguments(os);
    return os;
}

}
