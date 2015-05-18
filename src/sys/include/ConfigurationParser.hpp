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
    ConfigurationProperty(BaseArgument *argument, bool treatAsPath = false);
    ConfigurationProperty(ConfigurationProperty &&cp);
    ConfigurationProperty(const ConfigurationProperty &) = default;

    ConfigurationProperty & name(char const *name);

private:
    bool _treatAsPath;
    BaseArgument *_argument;
    std::string _name;
};

class ConfigurationParser
{
public:
    OperationResult parse(std::istream &is, const Path &filePath = nullptr);

    ConfigurationProperty &property(BaseArgument &arg);
    ConfigurationProperty &property(PathArg &arg);
    void validator(std::function<OperationResult()> validator);

private:
    std::vector<ConfigurationProperty> _configurationProperties;
    std::function<OperationResult()> _validator;
};

class ConfigurationFileArg : public PathArg
{
public:
    OperationResult convert(char const *value) override;

    ConfigurationParser &parser()
    {
        return _configurationParser;
    }

private:
    ConfigurationParser _configurationParser;
};

}

#endif
