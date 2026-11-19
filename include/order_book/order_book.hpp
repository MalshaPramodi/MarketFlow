#pragma once

#include "common/types.hpp"
#include "models/order.hpp"

#include <functional>
#include <list>
#include <map>
#include <optional>
#include <unordered_map>

namespace marketflow
{
    class OrderBook
    {
    private:
        using OrderQueue = std::list<Order>;

        using BidLevels = std::map<
            Price,
            OrderQueue,
            std::greater<Price>>;

        using AskLevels = std::map<
            Price,
            OrderQueue,
            std::less<Price>>;

        struct OrderLocation
        {
            OrderSide side;
            Price price;
            OrderQueue::iterator iterator;
        };

        InstrumentId instrumentId_;

        BidLevels bids_;
        AskLevels asks_;

        std::unordered_map<OrderId, OrderLocation> orderIndex_;

    public:
        explicit OrderBook(InstrumentId instrumentId);

        // Add an order to the appropriate side of the book.
        void addOrder(Order order);

        // Cancel and remove an active order.
        std::optional<Order> cancelOrder(const OrderId &orderId);

        // Remove an order that has been completely filled.
        bool removeFilledOrder(const OrderId &orderId);

        // Access the best available prices.
        std::optional<Price> getBestBidPrice() const;
        std::optional<Price> getBestAskPrice() const;

        // Access the best orders.
        Order *getBestBidOrder();
        Order *getBestAskOrder();

        const Order *getBestBidOrder() const;
        const Order *getBestAskOrder() const;

        // General information.
        const InstrumentId &getInstrumentId() const;
        std::size_t getOrderCount() const;
        bool empty() const;
    };
}
