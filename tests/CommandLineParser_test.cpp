#include <gtest/gtest.h>
#include "CommandLineParser.hpp"

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
    clp.option(arg).shortName("b");

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
    clp.option(arg).name("b");

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
    clp.option(arg).name("i");

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
    clp.option(arg).name("i");

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
    clp.option(arg).name("i");

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
    clp.option(arg).name("i");

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
    clp.option(arg).name("l");

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
    clp.option(arg).name("l");

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
    clp.option(arg).name("s");

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
    clp.option(arg).name("s");

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
    clp.option(arg).name("s");

    char const *argv[] = {"", "--s", "-70000"};
    bool result = clp.parse(3, argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
    ASSERT_EQ(0, arg.value());
}

TEST(CommandLineParser, canParseCharArgByName)
{
    sys::CharArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).name("character");

    char const *argv[] = {"", "--character", "z"};
    bool result = clp.parse(3, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_EQ('z', arg.value());
}

TEST(CommandLineParser, cannotParseCharArgWhenMoreThanOneChar)
{
    sys::CharArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).name("character");

    char const *argv[] = {"", "--character", "zz"};
    bool result = clp.parse(3, argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
}

TEST(CommandLineParser, canParseStringArgByName)
{
    sys::StringArg arg;

    sys::CommandLineParser clp;
    clp.option(arg).name("string");

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
    clp.option(arg).name("string");

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
    clp.option(arg1).name("arg1");
    clp.option(arg2).name("arg2");
    clp.option(arg3).name("arg3");
    clp.option(arg4).name("arg4").shortName("4");

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

TEST(CommandLineParser, canParseArguments)
{
    sys::CharSeqArg arg;

    sys::CommandLineParser clp;
    clp.argument(arg);

    char const *argv[] = {"", "hello"};
    bool result = clp.parse(2, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_STREQ("hello", arg.value());
}

TEST(CommandLineParser, cannotParseTooManyArguments)
{
    sys::CharSeqArg arg;

    sys::CommandLineParser clp;
    clp.argument(arg);

    char const *argv[] = {"", "hello", "world"};
    bool result = clp.parse(3, argv);

    ASSERT_FALSE(result);
}

TEST(CommandLineParser, canParseEnumeratedArguments)
{
    sys::EnumIntArg arg = {1,2,3};

    sys::CommandLineParser clp;
    clp.argument(arg);

    char const *argv[] = {"", "2"};
    bool result = clp.parse(2, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_EQ(2, arg.value());
}

TEST(CommandLineParser, canParseCharSeqEnumeratedArguments)
{
    sys::EnumCharSeqArg arg = {"a", "b", "c"};

    sys::CommandLineParser clp;
    clp.argument(arg);

    char const *argv[] = {"", "c"};
    bool result = clp.parse(2, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_STREQ("c", arg.value());
}


TEST(CommandLineParser, canParseStringEnumeratedArguments)
{
    sys::EnumStringArg arg = {"a", "b", "c"};

    sys::CommandLineParser clp;
    clp.argument(arg);

    char const *argv[] = {"", "a"};
    bool result = clp.parse(2, argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(arg);
    ASSERT_EQ(std::string("a"), arg.value());
}

TEST(CommandLineParser, cannotParseEnumeratedArguments)
{
    sys::EnumIntArg arg = {1,2,3};

    sys::CommandLineParser clp;
    clp.argument(arg);

    char const *argv[] = {"", "4"};
    bool result = clp.parse(2, argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(arg);
}