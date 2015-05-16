#include "gtest/gtest.h"
#include "PathArg.hpp"
#include "CommandLineParser.hpp"

using namespace sys;

TEST(PathArg, canParsePathArg)
{
    CommandLineParser clp;
    PathArg pathArg;
    clp.parameter(pathArg);

    const char *args[] = {"", "file.txt"};
    clp.parse(2, args);

    ASSERT_TRUE(pathArg);
    ASSERT_STREQ("file.txt", pathArg.value());
}

TEST(PathArg, canParsePathArgWithResolver)
{
    CommandLineParser clp;
    PathArg pathArg;

    clp.parameter(pathArg);
    pathArg.pathResolver([](Path &p){
        p = "resolved";
    });

    const char *args[] = {"", "file.txt"};
    clp.parse(2, args);

    ASSERT_TRUE(pathArg);
    ASSERT_STREQ("resolved", pathArg.value());
}

TEST(PathArg, canParsePathArgWithPathResolver)
{
    CommandLineParser clp;
    PathResolver pathResolver;
    PathArg pathArg;

    clp.parameter(pathArg);
    pathArg.pathResolver(pathResolver);
    pathResolver.pushParent(Path("dir/"));

    const char *args[] = {"", "file.txt"};
    clp.parse(2, args);

    ASSERT_TRUE(pathArg);
    ASSERT_STREQ("dir/file.txt", pathArg.value());
}
