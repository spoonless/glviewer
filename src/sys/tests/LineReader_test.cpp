#include <gtest/gtest.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include "LineReader.hpp"

using namespace sys;

TEST(LineReader, cannotReadUnknownFile)
{
    std::fstream f("unknown.file");
    LineReader lr(f);

    const char *line = lr.read();

    ASSERT_STREQ("", line);
    ASSERT_EQ(0u, lr.lineNumber());
    ASSERT_FALSE(lr);
}

TEST(LineReader, canReadLineWithoutEndLine)
{
    std::stringstream sstream("hello world");
    LineReader lr(sstream);

    const char *line = lr.read();

    ASSERT_STREQ("hello world", line);
    ASSERT_EQ(1u, lr.lineNumber());
    ASSERT_FALSE(lr);
}

TEST(LineReader, canTrimReadLines)
{
    std::stringstream sstream;
    sstream << "           hello world               " << std::endl;
    sstream << "\t\t\t\t\t\thello world\t\t\t\t\t\t\t" << std::endl;
    sstream << "hello world\r" << std::endl;
    LineReader lr(sstream);

    const char *line = lr.read();
    ASSERT_STREQ("hello world", line);

    line = lr.read();
    ASSERT_STREQ("hello world", line);

    line = lr.read();
    ASSERT_STREQ("hello world", line);
    ASSERT_EQ(3u, lr.lineNumber());
}

TEST(LineReader, canReadStreamUntilEof)
{
    std::stringstream sstream("hello world");
    LineReader lr(sstream);

    lr.read();

    ASSERT_FALSE(sstream.good());
    ASSERT_FALSE(sstream.bad());
    ASSERT_FALSE(sstream.fail());
    ASSERT_TRUE(sstream.eof());
}

TEST(LineReader, canReadLineWithEndLine)
{
    std::stringstream sstream("hello world\n");
    LineReader lr(sstream);

    const char *line = lr.read();

    ASSERT_STREQ("hello world", line);
    ASSERT_TRUE(lr);

    line = lr.read();

    ASSERT_STREQ("", line);
    ASSERT_FALSE(lr);
}

TEST(LineReader, canReadMultipleLines)
{
    std::stringstream sstream;
    sstream << "hello"      << std::endl;
    sstream << "multilines" << std::endl;
    sstream << "world";
    LineReader lr(sstream);

    const char *line = lr.read();

    ASSERT_STREQ("hello", line);
    ASSERT_TRUE(lr);

    line = lr.read();

    ASSERT_STREQ("multilines", line);
    ASSERT_TRUE(lr);

    line = lr.read();

    ASSERT_STREQ("world", line);
    ASSERT_FALSE(lr);
}

TEST(LineReader, canReadVeryLongLine)
{
    const std::size_t lineLength = 10 * 1024;
    std::string padding(lineLength, 'X');
    std::stringstream sstream(padding);

    LineReader lr(sstream);

    const char *line = lr.read();

    ASSERT_EQ(lineLength, std::strlen(line));
}

TEST(LineReader, canRemoveComments)
{
    std::stringstream sstream;
    sstream << "# comment"          << std::endl;
    sstream << "     # comment"     << std::endl;
    sstream << "hello #comment"     << std::endl;
    sstream << "hello# not comment" << std::endl;
    LineReader lr(sstream);

    const char *line = lr.read();
    ASSERT_STREQ("", line);

    line = lr.read();
    ASSERT_STREQ("", line);

    line = lr.read();
    ASSERT_STREQ("hello", line);

    line = lr.read();
    ASSERT_STREQ("hello# not comment", line);
}
