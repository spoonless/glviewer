#include <utility>
#include <gtest/gtest.h>
#include "Path.hpp"

using namespace sys;

TEST(Path, canMovePath)
{
    Path path("test");
    Path movedPath = std::move(path);

    ASSERT_STREQ("test", static_cast<const char*>(movedPath));
    ASSERT_STREQ("", static_cast<const char*>(path));
}

TEST(Path, canDetectAbsolute)
{
    ASSERT_FALSE(Path().absolute());
    ASSERT_FALSE(Path("test").absolute());
    ASSERT_FALSE(Path("test/test").absolute());
    ASSERT_TRUE(Path("/test/test").absolute());
    ASSERT_TRUE(Path("\\test").absolute());

    Path p("/");
    ASSERT_TRUE(p.absolute());
    p = "test";
    ASSERT_FALSE(p.absolute());
}

#ifdef _WIN32

TEST(Path, canDetectAbsoluteWin32)
{
    ASSERT_TRUE(Path("c:/test/test").absolute());
    ASSERT_TRUE(Path("zz:\\test\\test").absolute());
    ASSERT_TRUE(Path("\\\\test\\test").absolute());
    ASSERT_FALSE(Path("1:\\test\\test").absolute());
}

#else

TEST(Path, canDetectAbsoluteUnix)
{
    ASSERT_FALSE(Path("c:/test/test").absolute());
    ASSERT_FALSE(Path("zz:\\test\\test").absolute());
    ASSERT_TRUE(Path("\\\\test\\test").absolute());
    ASSERT_FALSE(Path("1:\\test\\test").absolute());
}

#endif

#ifdef _WIN32

TEST(Path, canNormalizePathWithNativeSeparatorWin32)
{
    Path p = "/test/file";
    ASSERT_STREQ("\\test\\file", static_cast<const char*>(p));
}

#else

TEST(Path, canNormalizePathWithNativeSeparatorUnix)
{
    Path p = "\\test\\file";
    ASSERT_STREQ("/test/file", static_cast<const char*>(p));
}

#endif

#ifdef _WIN32

TEST(Path, canConstructPathFromParentPathWin32)
{
    Path p("", "");
    ASSERT_STREQ("", static_cast<const char*>(p));


    p = Path("test", "/");
    ASSERT_STREQ("\\", static_cast<const char*>(p));

    p = Path("a/", "b");
    ASSERT_STREQ("a\\b", static_cast<const char*>(p));

    p = Path("a", "b");
    ASSERT_STREQ("a\\b", static_cast<const char*>(p));

    p = Path("a/", "../b");
    ASSERT_STREQ("a\\..\\b", static_cast<const char*>(p));
}

#else

TEST(Path, canConstructPathFromParentPathUnix)
{
    Path p("", "");
    ASSERT_STREQ("", static_cast<const char*>(p));


    p = Path("test", "/");
    ASSERT_STREQ("/", static_cast<const char*>(p));

    p = Path("a/", "b");
    ASSERT_STREQ("a/b", static_cast<const char*>(p));

    p = Path("a", "b");
    ASSERT_STREQ("a/b", static_cast<const char*>(p));

    p = Path("a/", "../b");
    ASSERT_STREQ("a/../b", static_cast<const char*>(p));

}

#endif

TEST(Path, canGetBasename)
{
    ASSERT_STREQ("", Path().basename());
    ASSERT_STREQ("", Path("a/b/c/").basename());
    ASSERT_STREQ("test", Path("test").basename());
    ASSERT_STREQ("test", Path("/test").basename());
    ASSERT_STREQ("test", Path("a/b/c/test").basename());
}

TEST(Path, canGetExtension)
{
    ASSERT_STREQ("", Path().extension());
    ASSERT_STREQ("", Path("a/b/c/").extension());
    ASSERT_STREQ("", Path("test").extension());
    ASSERT_STREQ("", Path(".test").extension());
    ASSERT_STREQ("", Path("/a/b/.test").extension());
    ASSERT_STREQ("txt", Path("test.txt").extension());
    ASSERT_STREQ("txt", Path("a/b/c/test.txt").extension());
    ASSERT_STREQ("", Path("/a/b/c.txt/").extension());
    ASSERT_STREQ("", Path("/a/b/c.txt/d").extension());
}

TEST(Path, canGetWithoutExtension)
{
    ASSERT_STREQ("", static_cast<const char*>(Path().withoutExtension()));
    ASSERT_STREQ("test", static_cast<const char*>(Path("test").withoutExtension()));
    ASSERT_STREQ("test", static_cast<const char*>(Path("test.txt").withoutExtension()));
    ASSERT_STREQ("test.txt", static_cast<const char*>(Path("test.txt.txt").withoutExtension()));
    ASSERT_STREQ(".txt", static_cast<const char*>(Path(".txt").withoutExtension()));
}

#ifdef _WIN32

TEST(Path, canGetWithoutExtensionWin32)
{
    ASSERT_STREQ("a\\.txt", static_cast<const char*>(Path("a/.txt").withoutExtension()));
}

#else

TEST(Path, canGetWithoutExtensionUnix)
{
    ASSERT_STREQ("a/.txt", static_cast<const char*>(Path("a/.txt").withoutExtension()));
}

#endif

TEST(Path, canGetDirpath)
{
    ASSERT_STREQ(".", static_cast<const char*>(Path().dirpath()));
    ASSERT_STREQ(".", static_cast<const char*>(Path(".").dirpath()));
    ASSERT_STREQ(".", static_cast<const char*>(Path("test").dirpath()));
    ASSERT_STREQ(".", static_cast<const char*>(Path("test/").dirpath()));
    ASSERT_STREQ("a", static_cast<const char*>(Path("a/test").dirpath()));
    ASSERT_STREQ("a", static_cast<const char*>(Path("a/test/").dirpath()));

#ifdef _WIN32
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

#ifdef _WIN32

TEST(Path, canGetDirpathWin32)
{
    ASSERT_STREQ("a\\b", static_cast<const char*>(Path("a/b/test").dirpath()));
    ASSERT_STREQ("\\a\\b", static_cast<const char*>(Path("/a/b/test").dirpath()));
    ASSERT_STREQ("\\", static_cast<const char*>(Path("/test").dirpath()));
    ASSERT_STREQ("c:\\", static_cast<const char*>(Path("c:/test").dirpath()));
    ASSERT_STREQ("c:\\test", static_cast<const char*>(Path("c:/test/a").dirpath()));
    ASSERT_STREQ("c:\\", static_cast<const char*>(Path("c:/").dirpath()));
    ASSERT_STREQ(".", static_cast<const char*>(Path("c:").dirpath()));
}

#else

TEST(Path, canGetDirpathUnix)
{
    ASSERT_STREQ("a/b", static_cast<const char*>(Path("a/b/test").dirpath()));
    ASSERT_STREQ("/a/b", static_cast<const char*>(Path("/a/b/test").dirpath()));
    ASSERT_STREQ("/", static_cast<const char*>(Path("/test").dirpath()));
}

#endif
