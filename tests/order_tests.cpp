#include <gtest/gtest.h>
#include <stdexcept>
#include "models/order.hpp"

using namespace marketflow;

TEST(OrderTest, NewOrderHasCorrectInitialValues)
{
    Order order("ORD001", "AAPL", OrderSide::Buy, 19025, 100, 1);

    EXPECT_EQ(order.getOrderId(), "ORD001");
    EXPECT_EQ(order.getInstrumentId(), "AAPL");
    EXPECT_EQ(order.getSide(), OrderSide::Buy);
    EXPECT_EQ(order.getPrice(), 19025);
    EXPECT_EQ(order.getQuantity(), 100);
    EXPECT_EQ(order.getRemainingQuantity(), 100);
    EXPECT_EQ(order.getStatus(), OrderStatus::New);
}

TEST(OrderTest, RejectsEmptyOrderId)
{
    EXPECT_THROW(
        Order("", "AAPL", OrderSide::Buy, 19025, 100, 1),
        std::invalid_argument);
}

TEST(OrderTest, RejectsEmptyInstrumentId)
{
    EXPECT_THROW(
        Order("ORD001", "", OrderSide::Buy, 19025, 100, 1),
        std::invalid_argument);
}

TEST(OrderTest, RejectsNonPositivePrice)
{
    EXPECT_THROW(
        Order("ORD001", "AAPL", OrderSide::Buy, 0, 100, 1),
        std::invalid_argument);
}

TEST(OrderTest, RejectsZeroQuantity)
{
    EXPECT_THROW(
        Order("ORD001", "AAPL", OrderSide::Buy, 19025, 0, 1),
        std::invalid_argument);
}

TEST(OrderTest, CanBeMarkedOpen)
{
    Order order("ORD001", "AAPL", OrderSide::Buy, 19025, 100, 1);

    order.markOpen();

    EXPECT_EQ(order.getStatus(), OrderStatus::Open);
    EXPECT_TRUE(order.isActive());
}

TEST(OrderTest, SupportsPartialFill)
{
    Order order("ORD001", "AAPL", OrderSide::Buy, 19025, 100, 1);

    order.markOpen();
    order.applyFill(40);

    EXPECT_EQ(order.getRemainingQuantity(), 60);
    EXPECT_EQ(order.getStatus(), OrderStatus::PartiallyFilled);
    EXPECT_TRUE(order.isActive());
}

TEST(OrderTest, IncomingNewOrderCanBeFilledImmediately)
{
    Order order("ORD001", "AAPL", OrderSide::Buy, 19025, 100, 1);

    order.applyFill(100);

    EXPECT_EQ(order.getRemainingQuantity(), 0);
    EXPECT_EQ(order.getStatus(), OrderStatus::Filled);
    EXPECT_FALSE(order.isActive());
}

TEST(OrderTest, SupportsFullFillAfterPartialFill)
{
    Order order("ORD001", "AAPL", OrderSide::Buy, 19025, 100, 1);

    order.markOpen();
    order.applyFill(40);
    order.applyFill(60);

    EXPECT_EQ(order.getRemainingQuantity(), 0);
    EXPECT_EQ(order.getStatus(), OrderStatus::Filled);
}

TEST(OrderTest, SupportsCancellation)
{
    Order order("ORD001", "AAPL", OrderSide::Buy, 19025, 100, 1);

    order.markOpen();
    order.cancel();

    EXPECT_EQ(order.getStatus(), OrderStatus::Cancelled);
    EXPECT_FALSE(order.isActive());
}

TEST(OrderTest, SupportsCancellationAfterPartialFill)
{
    Order order("ORD001", "AAPL", OrderSide::Buy, 19025, 100, 1);

    order.markOpen();
    order.applyFill(40);
    order.cancel();

    EXPECT_EQ(order.getRemainingQuantity(), 60);
    EXPECT_EQ(order.getStatus(), OrderStatus::Cancelled);
}

TEST(OrderTest, SupportsRejectionOfNewOrder)
{
    Order order("ORD001", "AAPL", OrderSide::Buy, 19025, 100, 1);

    order.reject();

    EXPECT_EQ(order.getStatus(), OrderStatus::Rejected);
    EXPECT_FALSE(order.isActive());
}

TEST(OrderTest, RejectsFillGreaterThanRemainingQuantity)
{
    Order order("ORD001", "AAPL", OrderSide::Buy, 19025, 100, 1);

    order.markOpen();

    EXPECT_THROW(order.applyFill(101), std::invalid_argument);
}

TEST(OrderTest, RejectsCancellationOfFilledOrder)
{
    Order order("ORD001", "AAPL", OrderSide::Buy, 19025, 100, 1);

    order.applyFill(100);

    EXPECT_THROW(order.cancel(), std::logic_error);
}
