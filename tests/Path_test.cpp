#include <gtest/gtest.h>
#include "Path.hpp"

using namespace sys;

TEST(Path, canDetectAbsolute)
{
    ASSERT_FALSE(Path().absolute());
    ASSERT_FALSE(Path("test").absolute());
    ASSERT_FALSE(Path("test/test").absolute());
    ASSERT_TRUE(Path("/test/test").absolute());
    ASSERT_TRUE(Path("\\test").absolute());

#ifdef WIN32
    ASSERT_TRUE(Path("c:/test/test").absolute());
    ASSERT_TRUE(Path("zz:\\test\\test").absolute());
    ASSERT_TRUE(Path("\\\\test\\test").absolute());
    ASSERT_FALSE(Path("1:\\test\\test").absolute());
#else
    ASSERT_FALSE(Path("c:/test/test").absolute());
    ASSERT_FALSE(Path("zz:\\test\\test").absolute());
    ASSERT_TRUE(Path("\\\\test\\test").absolute());
    ASSERT_FALSE(Path("1:\\test\\test").absolute());
#endif

    Path p("/");
    ASSERT_TRUE(p.absolute());
    p = "test";
    ASSERT_FALSE(p.absolute());
}

TEST(Path, canNormalizePathWithNativeSeparator)
{
#ifdef WIN32
    Path p = "/test/file";
    ASSERT_STREQ("\\test\\file", static_cast<const char*>(p));
#else
    Path p = "\\test\\file";
    ASSERT_STREQ("/test/file", static_cast<const char*>(p));
#endif

}

TEST(Path, canConstructPathFromParentPath)
{
    Path p("", "");
    ASSERT_STREQ("", static_cast<const char*>(p));

#ifdef WIN32

    p = Path("test", "/");
    ASSERT_STREQ("\\", static_cast<const char*>(p));

    p = Path("a/", "b");
    ASSERT_STREQ("a\\b", static_cast<const char*>(p));

    p = Path("a", "b");
    ASSERT_STREQ("a\\b", static_cast<const char*>(p));

    p = Path("a/", "../b");
    ASSERT_STREQ("a\\..\\b", static_cast<const char*>(p));

#else

    p = Path("test", "/");
    ASSERT_STREQ("/", static_cast<const char*>(p));

    p = Path("a/", "b");
    ASSERT_STREQ("a/b", static_cast<const char*>(p));

    p = Path("a", "b");
    ASSERT_STREQ("a/b", static_cast<const char*>(p));

    p = Path("a/", "../b");
    ASSERT_STREQ("a/../b", static_cast<const char*>(p));

#endif
}

TEST(Path, canGetBasename)
{
    ASSERT_STREQ("", Path().basename());
    ASSERT_STREQ("", Path("a/b/c/").basename());
    ASSERT_STREQ("test", Path("test").basename());
    ASSERT_STREQ("test", Path("/test").basename());
    ASSERT_STREQ("test", Path("a/b/c/test").basename());
}

TEST(Path, canGetDirpath)
{
    ASSERT_STREQ(".", static_cast<const char*>(Path().dirpath()));
    ASSERT_STREQ(".", static_cast<const char*>(Path(".").dirpath()));
    ASSERT_STREQ(".", static_cast<const char*>(Path("test").dirpath()));
    ASSERT_STREQ(".", static_cast<const char*>(Path("test/").dirpath()));
    ASSERT_STREQ("a", static_cast<const char*>(Path("a/test").dirpath()));
    ASSERT_STREQ("a", static_cast<const char*>(Path("a/test/").dirpath()));

#ifdef WIN32
    ASSERT_STREQ("a\\b", static_cast<const char*>(Path("a/b/test").dirpath()));
    ASSERT_STREQ("\\a\\b", static_cast<const char*>(Path("/a/b/test").dirpath()));
    ASSERT_STREQ("\\", static_cast<const char*>(Path("/test").dirpath()));
    ASSERT_STREQ("c:\\", static_cast<const char*>(Path("c:/test").dirpath()));
    ASSERT_STREQ("c:\\test", static_cast<const char*>(Path("c:/test/a").dirpath()));
    ASSERT_STREQ("c:\\", static_cast<const char*>(Path("c:/").dirpath()));
    ASSERT_STREQ(".", static_cast<const char*>(Path("c:").dirpath()));
#else
    ASSERT_STREQ("a/b", static_cast<const char*>(Path("a/b/test").dirpath()));
    ASSERT_STREQ("/a/b", static_cast<const char*>(Path("/a/b/test").dirpath()));
    ASSERT_STREQ("/", static_cast<const char*>(Path("/test").dirpath()));
#endif
}
