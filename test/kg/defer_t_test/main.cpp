#include <gtest/gtest.h>
#include <kg/defer.hpp>

TEST(defer_t_test, HandleNoneZeroInput)
{
    int x = 0;
    kg::defer_t([&]()
    {
        EXPECT_EQ(x++,0);
    });

    {
        kg::defer_t([&]()
        {
            EXPECT_EQ(x++,1);
        });
    }

    kg::defer_t([&]()
    {
        EXPECT_EQ(x++,2);
    });
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

