#ifndef CONFIGURATION_PARSER_HPP
#define CONFIGURATION_PARSER_HPP

#include <istream>
#include <functional>
#include <vector>
#include <string>
#include "Argument.hpp"

namespace sys
{

class ConfigurationProperty
{
    friend class ConfigurationParser;
public:
    ConfigurationProperty(BaseArgument *argument);
    ConfigurationProperty(ConfigurationProperty &&cp);
    ConfigurationProperty(const ConfigurationProperty &) = default;

    ConfigurationProperty & name(char const *name);

private:
    BaseArgument *_argument;
    std::string _name;
};

class ConfigurationParser
{
public:
    OperationResult parse(std::istream &is);

    ConfigurationProperty &property(BaseArgument &arg);
    void validator(std::function<OperationResult()> validator);

private:
    std::vector<ConfigurationProperty> _configurationProperties;
    std::function<OperationResult()> _validator;
};

}

#endif
