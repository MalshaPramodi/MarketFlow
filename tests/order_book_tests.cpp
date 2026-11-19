#include <gtest/gtest.h>

#include "order_book/order_book.hpp"
#include <utility>
#include <stdexcept>

using namespace marketflow;

class OrderBookTest : public ::testing::Test
{
protected:
    OrderBook book{"AAPL"};

    Order createOrder(
        const std::string &id,
        OrderSide side,
        Price price,
        Quantity quantity,
        SequenceNumber sequence)
    {
        return Order(
            id,
            "AAPL",
            side,
            price,
            quantity,
            sequence);
    }
};

TEST_F(OrderBookTest, StartsEmpty)
{
    EXPECT_TRUE(book.empty());
    EXPECT_EQ(book.getOrderCount(), 0);
    EXPECT_FALSE(book.getBestBidPrice().has_value());
    EXPECT_FALSE(book.getBestAskPrice().has_value());
}

TEST_F(OrderBookTest, AddsBuyOrder)
{
    book.addOrder(
        createOrder("BUY001", OrderSide::Buy, 19000, 100, 1));

    EXPECT_EQ(book.getOrderCount(), 1);
    EXPECT_EQ(book.getBestBidPrice(), 19000);
}

TEST_F(OrderBookTest, AddsSellOrder)
{
    book.addOrder(
        createOrder("SELL001", OrderSide::Sell, 19100, 100, 1));

    EXPECT_EQ(book.getOrderCount(), 1);
    EXPECT_EQ(book.getBestAskPrice(), 19100);
}

TEST_F(OrderBookTest, BuySidePrioritizesHighestPrice)
{
    book.addOrder(
        createOrder("BUY001", OrderSide::Buy, 19000, 100, 1));

    book.addOrder(
        createOrder("BUY002", OrderSide::Buy, 19200, 100, 2));

    book.addOrder(
        createOrder("BUY003", OrderSide::Buy, 19100, 100, 3));

    EXPECT_EQ(book.getBestBidPrice(), 19200);
}

TEST_F(OrderBookTest, SellSidePrioritizesLowestPrice)
{
    book.addOrder(
        createOrder("SELL001", OrderSide::Sell, 19200, 100, 1));

    book.addOrder(
        createOrder("SELL002", OrderSide::Sell, 19000, 100, 2));

    book.addOrder(
        createOrder("SELL003", OrderSide::Sell, 19100, 100, 3));

    EXPECT_EQ(book.getBestAskPrice(), 19000);
}

TEST_F(OrderBookTest, BuyOrdersAtSamePriceFollowFifo)
{
    book.addOrder(
        createOrder("BUY001", OrderSide::Buy, 19000, 100, 1));

    book.addOrder(
        createOrder("BUY002", OrderSide::Buy, 19000, 100, 2));

    ASSERT_NE(book.getBestBidOrder(), nullptr);

    EXPECT_EQ(
        book.getBestBidOrder()->getOrderId(),
        "BUY001");
}

TEST_F(OrderBookTest, SellOrdersAtSamePriceFollowFifo)
{
    book.addOrder(
        createOrder("SELL001", OrderSide::Sell, 19100, 100, 1));

    book.addOrder(
        createOrder("SELL002", OrderSide::Sell, 19100, 100, 2));

    ASSERT_NE(book.getBestAskOrder(), nullptr);

    EXPECT_EQ(
        book.getBestAskOrder()->getOrderId(),
        "SELL001");
}

TEST_F(OrderBookTest, NewOrdersBecomeOpenWhenAdded)
{
    book.addOrder(
        createOrder("BUY001", OrderSide::Buy, 19000, 100, 1));

    ASSERT_NE(book.getBestBidOrder(), nullptr);

    EXPECT_EQ(
        book.getBestBidOrder()->getStatus(),
        OrderStatus::Open);
}

TEST_F(OrderBookTest, RejectsDuplicateOrderId)
{
    book.addOrder(
        createOrder("ORD001", OrderSide::Buy, 19000, 100, 1));

    EXPECT_THROW(
        book.addOrder(
            createOrder("ORD001", OrderSide::Sell, 19100, 100, 2)),
        std::invalid_argument);

    EXPECT_EQ(book.getOrderCount(), 1);
}

TEST_F(OrderBookTest, RejectsDifferentInstrument)
{
    Order otherInstrumentOrder(
        "ORD002",
        "MSFT",
        OrderSide::Buy,
        19000,
        100,
        1);

    EXPECT_THROW(
        book.addOrder(std::move(otherInstrumentOrder)),
        std::invalid_argument);

    EXPECT_TRUE(book.empty());
}

TEST_F(OrderBookTest, CancelsExistingOrder)
{
    book.addOrder(
        createOrder("BUY001", OrderSide::Buy, 19000, 100, 1));

    auto cancelledOrder = book.cancelOrder("BUY001");

    ASSERT_TRUE(cancelledOrder.has_value());

    EXPECT_EQ(
        cancelledOrder->getStatus(),
        OrderStatus::Cancelled);

    EXPECT_TRUE(book.empty());
}

TEST_F(OrderBookTest, CancellingUnknownOrderReturnsEmpty)
{
    auto result = book.cancelOrder("UNKNOWN");

    EXPECT_FALSE(result.has_value());
}

TEST_F(OrderBookTest, CancellationRemovesEmptyPriceLevel)
{
    book.addOrder(
        createOrder("BUY001", OrderSide::Buy, 19000, 100, 1));

    book.addOrder(
        createOrder("BUY002", OrderSide::Buy, 19100, 100, 2));

    book.cancelOrder("BUY002");

    EXPECT_EQ(book.getBestBidPrice(), 19000);
    EXPECT_EQ(book.getOrderCount(), 1);
}

TEST_F(OrderBookTest, RemovesFilledOrder)
{
    book.addOrder(
        createOrder("BUY001", OrderSide::Buy, 19000, 100, 1));

    Order *bestOrder = book.getBestBidOrder();

    ASSERT_NE(bestOrder, nullptr);

    bestOrder->applyFill(100);

    EXPECT_TRUE(book.removeFilledOrder("BUY001"));
    EXPECT_TRUE(book.empty());
}

TEST_F(OrderBookTest, DoesNotRemoveUnfilledOrderAsFilled)
{
    book.addOrder(
        createOrder("BUY001", OrderSide::Buy, 19000, 100, 1));

    EXPECT_FALSE(book.removeFilledOrder("BUY001"));
    EXPECT_EQ(book.getOrderCount(), 1);
}
