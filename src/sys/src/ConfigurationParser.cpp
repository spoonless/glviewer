#include <cctype>
#include "ConfigurationParser.hpp"
#include "LineReader.hpp"

namespace
{

inline std::size_t findChar(const char *line, char c)
{
    std::size_t index = 0;
    for (;line[index] != 0 && line[index] != c; ++index);
    return index;
}


inline std::size_t ignoreTrailingWithspace(const char *line, std::size_t length)
{
    std::size_t index = length;
    for (; index > 0 && std::isspace(line[index-1]); --index);
    return index;
}

inline std::size_t ignoreLeadingWithspace(const char *line, std::size_t startPos)
{
    std::size_t index = startPos;
    for(; line[index] != 0 && std::isspace(line[index]); ++index);
    return index;
}

const char *PROPERTY_VALUE_SEPARATOR_SIGN = "=";

}

namespace sys
{

ConfigurationProperty::ConfigurationProperty(BaseArgument *argument)
    : _argument(argument)
{
}

ConfigurationProperty::ConfigurationProperty(ConfigurationProperty &&cp)
    : _argument(cp._argument), _name(std::move(cp._name))
{
}

ConfigurationProperty & ConfigurationProperty::name(char const *name)
{
    _name = name;
    return *this;
}

OperationResult ConfigurationParser::parse(std::istream &is)
{
    LineReader lr(is);
    do
    {
        const char *line = lr.read();
        if (*line == 0)
        {
            continue;
        }

        std::size_t equalSignPos = findChar(line, *PROPERTY_VALUE_SEPARATOR_SIGN);
        if (line[equalSignPos] == 0)
        {
            std::string msg;
            msg.append("Missing '").append(PROPERTY_VALUE_SEPARATOR_SIGN).append("' character at line ").append(std::to_string(lr.lineNumber()));
            return OperationResult::failed(msg);
        }

        std::size_t propertyNameSize = ignoreTrailingWithspace(line, equalSignPos);
        if (propertyNameSize == 0)
        {
            std::string msg;
            msg.append("Unexpected '").append(PROPERTY_VALUE_SEPARATOR_SIGN).append("' character at line ").append(std::to_string(lr.lineNumber()));
            return OperationResult::failed(msg);
        }

        for(ConfigurationProperty cp : _configurationProperties)
        {
            if (cp._name.compare(0, cp._name.size(), line, propertyNameSize) == 0)
            {
                std::size_t propertyValuePos = ignoreLeadingWithspace(line, equalSignPos +1);
                OperationResult result = cp._argument->convert(line+propertyValuePos);
                if (!result)
                {
                    std::string msg;
                    msg.append("Cannot convert property value of '").append(cp._name).append("' at line ").append(std::to_string(lr.lineNumber()));
                    return OperationResult::failed(msg);
                }
                break;
            }
        }
    }
    while(lr);

    if (is.fail())
    {
        return OperationResult::failed("I/O error while reading configuration!");
    }
    if (_validator)
    {
        return _validator();
    }
    return OperationResult::succeeded();
}

ConfigurationProperty &ConfigurationParser::property(BaseArgument &arg)
{
    _configurationProperties.push_back(ConfigurationProperty(&arg));
    return _configurationProperties.back();
}

void ConfigurationParser::validator(std::function<OperationResult()> validator)
{
    _validator = validator;
}


}
