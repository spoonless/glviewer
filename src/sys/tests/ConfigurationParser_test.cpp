#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include "ConfigurationParser.hpp"

using namespace sys;

TEST(ConfigurationParser, cannotParseUnknownFile)
{
    std::ifstream ifs("unknown.file");
    ConfigurationParser cp;

    OperationResult result = cp.parse(ifs);

    ASSERT_FALSE(result);
}

TEST(ConfigurationParser, canParseEmptyFile)
{
    std::stringstream sstream;
    ConfigurationParser cp;

    OperationResult result = cp.parse(sstream);

    ASSERT_TRUE(result);
}

TEST(ConfigurationParser, canParseProperties)
{
    CharSeqArg arg1;
    IntArg arg2;
    std::stringstream sstream;
    sstream << "my_property=test" << std::endl;
    sstream << "my_property2=10" << std::endl;

    ConfigurationParser cp;
    cp.property(arg1).name("my_property");
    cp.property(arg2).name("my_property2");

    OperationResult result = cp.parse(sstream);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg1);
    ASSERT_TRUE(arg2);
    ASSERT_STREQ("test", arg1.value());
    ASSERT_EQ(10, arg2.value());
}

TEST(ConfigurationParser, canParsePropertyWithLeadingAndTrailingWhitespaces)
{
    CharSeqArg arg;
    std::stringstream sstream;
    sstream << "     my_property    =   test    " << std::endl;

    ConfigurationParser cp;
    cp.property(arg).name("my_property");

    OperationResult result = cp.parse(sstream);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_STREQ("test", arg.value());
}

TEST(ConfigurationParser, cannotParseInvalidPropertyValue)
{
    EnumCharSeqArg arg={"house", "village"};
    std::stringstream sstream;
    sstream << "my_property=city" << std::endl;

    ConfigurationParser cp;
    cp.property(arg).name("my_property");

    OperationResult result = cp.parse(sstream);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
    ASSERT_EQ(std::string("Cannot convert property value of 'my_property' at line 1. Reason is: value does not match any possible values"), result.message());
}

TEST(ConfigurationParser, cannotParseLineWithoutEqualSign)
{
    std::stringstream sstream;
    sstream << " test" << std::endl;

    ConfigurationParser cp;

    OperationResult result = cp.parse(sstream);

    ASSERT_FALSE(result);
    ASSERT_EQ(std::string("Missing '=' character at line 1"), result.message());
}

TEST(ConfigurationParser, cannotParseLineBeginningWithEqualSign)
{
    std::stringstream sstream;
    sstream << " = test" << std::endl;

    ConfigurationParser cp;

    OperationResult result = cp.parse(sstream);

    ASSERT_FALSE(result);
    ASSERT_EQ(std::string("Unexpected '=' character at line 1"), result.message());
}

TEST(ConfigurationParser, canIgnoreEmptyLineOrCommentedLine)
{
    CharSeqArg arg;
    std::stringstream sstream;
    sstream << std::endl;
    sstream << std::endl;
    sstream << "               " << std::endl;
    sstream << "\t#commented line" << std::endl;
    sstream << "my_property=test # another comment" << std::endl;
    sstream << "               " << std::endl;
    sstream << "#commented line" << std::endl;

    ConfigurationParser cp;
    cp.property(arg).name("my_property");

    OperationResult result = cp.parse(sstream);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_STREQ("test", arg.value());
}

TEST(ConfigurationParser, canParseBooleanArgToTrue)
{
    BoolArg args[7];
    std::stringstream sstream;
    sstream << "arg1=ON" << std::endl;
    sstream << "arg2=on" << std::endl;
    sstream << "arg3=TRUE" << std::endl;
    sstream << "arg4=true" << std::endl;
    sstream << "arg5=1" << std::endl;
    sstream << "arg6=yes" << std::endl;
    sstream << "arg7=YES" << std::endl;

    ConfigurationParser cp;
    cp.property(args[0]).name("arg1");
    cp.property(args[1]).name("arg2");
    cp.property(args[2]).name("arg3");
    cp.property(args[3]).name("arg4");
    cp.property(args[4]).name("arg5");
    cp.property(args[5]).name("arg6");
    cp.property(args[6]).name("arg7");

    OperationResult result = cp.parse(sstream);

    ASSERT_TRUE(result);
    ASSERT_TRUE(args[0].value());
    ASSERT_TRUE(args[1].value());
    ASSERT_TRUE(args[2].value());
    ASSERT_TRUE(args[3].value());
    ASSERT_TRUE(args[4].value());
    ASSERT_TRUE(args[5].value());
    ASSERT_TRUE(args[6].value());
}

TEST(ConfigurationParser, canParseBooleanArgToFalse)
{
    BoolArg args[7];
    std::stringstream sstream;
    sstream << "arg1=OFF" << std::endl;
    sstream << "arg2=off" << std::endl;
    sstream << "arg3=false" << std::endl;
    sstream << "arg4=FALSE" << std::endl;
    sstream << "arg5=0" << std::endl;
    sstream << "arg6=no" << std::endl;
    sstream << "arg7=NO" << std::endl;

    ConfigurationParser cp;
    cp.property(args[0]).name("arg1");
    cp.property(args[1]).name("arg2");
    cp.property(args[2]).name("arg3");
    cp.property(args[3]).name("arg4");
    cp.property(args[4]).name("arg5");
    cp.property(args[5]).name("arg6");
    cp.property(args[6]).name("arg7");

    OperationResult result = cp.parse(sstream);

    ASSERT_TRUE(result);
    ASSERT_FALSE(args[0].value());
    ASSERT_FALSE(args[1].value());
    ASSERT_FALSE(args[2].value());
    ASSERT_FALSE(args[3].value());
    ASSERT_FALSE(args[4].value());
    ASSERT_FALSE(args[5].value());
    ASSERT_FALSE(args[6].value());
}

TEST(ConfigurationParser, canUseValidator)
{
    std::stringstream sstream;
    ConfigurationParser cp;
    cp.validator([](){
        return OperationResult::failed("validator failed");
    });

    OperationResult result = cp.parse(sstream);

    ASSERT_FALSE(result);
    ASSERT_EQ(std::string("validator failed"), result.message());
}

TEST(ConfigurationParser, canParsePathArg)
{
    std::stringstream sstream;
    sstream << "prop=file.txt";
    PathArg arg;
    ConfigurationParser cp;
    cp.property(arg).name("prop");

    cp.parse(sstream);

    ASSERT_TRUE(arg);
    ASSERT_STREQ(Path(Path().dirpath(),"file.txt"), arg.value());
}

TEST(ConfigurationParser, canParsePathArgWithFilePath)
{
    std::stringstream sstream;
    sstream << "prop=file.txt";
    PathArg arg;
    ConfigurationParser cp;
    cp.property(arg).name("prop");
    Path filePath("/tmp/exe");

    cp.parse(sstream, filePath);

    ASSERT_TRUE(arg);
    ASSERT_STREQ(Path(filePath.dirpath(), "file.txt"), arg.value());
}

TEST(ConfigurationParser, cannotParseConfigurationFileArgWhenConfigurationFileDoesNotExist)
{
    std::stringstream sstream;
    sstream << "conf=unknown_file.txt";
    ConfigurationFileArg arg;
    ConfigurationParser cp;
    Path filepath("test");

    cp.property(arg).name("conf");

    OperationResult result = cp.parse(sstream, filepath);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
    std::string expectedMsg;
    expectedMsg.append("Cannot convert property value of 'conf' at line 1. ")
               .append("Reason is: Error while loading configuration file '")
               .append(Path(filepath.dirpath(), "unknown_file.txt")).append("'! ")
               .append("I/O error while reading configuration! ")
               .append("Maybe the file does not exist or is not readable.");

    ASSERT_EQ(expectedMsg, result.message());
}
