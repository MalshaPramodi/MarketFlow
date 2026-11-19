#include <gtest/gtest.h>

// Basic test to verify GoogleTest integration.
TEST(MarketFlowSetupTest, BasicAssertion)
{
    EXPECT_EQ(2 + 2, 4);
}

// Verify that the application uses the C++20 standard.
TEST(MarketFlowSetupTest, UsesCpp20)
{
    EXPECT_EQ(__cplusplus, 202002L);
}
