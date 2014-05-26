#include <gtest/gtest.h>

int add(int x, int y);

TEST(sample, canAdd)
{
    int result = add(2, 3);

    ASSERT_TRUE(result);
    ASSERT_EQ(5, result);
}
