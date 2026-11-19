#include "order_book/order_book.hpp"
#include <iterator>
#include <stdexcept>
#include <utility>

namespace marketflow
{
    OrderBook::OrderBook(InstrumentId instrumentId)
        : instrumentId_(std::move(instrumentId))
    {
        if (instrumentId_.empty())
        {
            throw std::invalid_argument(
                "Order book instrument ID cannot be empty");
        }
    }

    void OrderBook::addOrder(Order order)
    {
        if (order.getInstrumentId() != instrumentId_)
        {
            throw std::invalid_argument(
                "Order instrument does not match order book");
        }

        if (orderIndex_.find(order.getOrderId()) != orderIndex_.end())
        {
            throw std::invalid_argument(
                "Duplicate order ID in order book");
        }

        if (order.getStatus() == OrderStatus::New)
        {
            order.markOpen();
        }
        else if (!order.isActive())
        {
            throw std::invalid_argument(
                "Only new or active orders can be added");
        }

        const OrderId orderId = order.getOrderId();
        const Price price = order.getPrice();
        const OrderSide side = order.getSide();

        if (side == OrderSide::Buy)
        {
            auto levelIt = bids_.try_emplace(price).first;

            try
            {
                levelIt->second.push_back(std::move(order));

                auto orderIt = std::prev(levelIt->second.end());

                try
                {
                    orderIndex_.emplace(
                        orderId,
                        OrderLocation{side, price, orderIt});
                }
                catch (...)
                {
                    levelIt->second.erase(orderIt);
                    throw;
                }
            }
            catch (...)
            {
                if (levelIt->second.empty())
                {
                    bids_.erase(levelIt);
                }

                throw;
            }
        }
        else
        {
            auto levelIt = asks_.try_emplace(price).first;

            try
            {
                levelIt->second.push_back(std::move(order));

                auto orderIt = std::prev(levelIt->second.end());

                try
                {
                    orderIndex_.emplace(
                        orderId,
                        OrderLocation{side, price, orderIt});
                }
                catch (...)
                {
                    levelIt->second.erase(orderIt);
                    throw;
                }
            }
            catch (...)
            {
                if (levelIt->second.empty())
                {
                    asks_.erase(levelIt);
                }

                throw;
            }
        }
    }

    std::optional<Order> OrderBook::cancelOrder(
        const OrderId &orderId)
    {
        auto indexIt = orderIndex_.find(orderId);

        if (indexIt == orderIndex_.end())
        {
            return std::nullopt;
        }

        const OrderLocation location = indexIt->second;

        Order cancelledOrder = *location.iterator;
        cancelledOrder.cancel();

        if (location.side == OrderSide::Buy)
        {
            auto levelIt = bids_.find(location.price);

            levelIt->second.erase(location.iterator);

            if (levelIt->second.empty())
            {
                bids_.erase(levelIt);
            }
        }
        else
        {
            auto levelIt = asks_.find(location.price);

            levelIt->second.erase(location.iterator);

            if (levelIt->second.empty())
            {
                asks_.erase(levelIt);
            }
        }

        orderIndex_.erase(indexIt);

        return cancelledOrder;
    }

    bool OrderBook::removeFilledOrder(const OrderId &orderId)
    {
        auto indexIt = orderIndex_.find(orderId);

        if (indexIt == orderIndex_.end())
        {
            return false;
        }

        const OrderLocation location = indexIt->second;

        if (location.iterator->getStatus() != OrderStatus::Filled)
        {
            return false;
        }

        if (location.side == OrderSide::Buy)
        {
            auto levelIt = bids_.find(location.price);

            levelIt->second.erase(location.iterator);

            if (levelIt->second.empty())
            {
                bids_.erase(levelIt);
            }
        }
        else
        {
            auto levelIt = asks_.find(location.price);

            levelIt->second.erase(location.iterator);

            if (levelIt->second.empty())
            {
                asks_.erase(levelIt);
            }
        }

        orderIndex_.erase(indexIt);

        return true;
    }

    std::optional<Price> OrderBook::getBestBidPrice() const
    {
        if (bids_.empty())
        {
            return std::nullopt;
        }

        return bids_.begin()->first;
    }

    std::optional<Price> OrderBook::getBestAskPrice() const
    {
        if (asks_.empty())
        {
            return std::nullopt;
        }

        return asks_.begin()->first;
    }

    Order *OrderBook::getBestBidOrder()
    {
        if (bids_.empty())
        {
            return nullptr;
        }

        return &bids_.begin()->second.front();
    }

    Order *OrderBook::getBestAskOrder()
    {
        if (asks_.empty())
        {
            return nullptr;
        }

        return &asks_.begin()->second.front();
    }

    const Order *OrderBook::getBestBidOrder() const
    {
        if (bids_.empty())
        {
            return nullptr;
        }

        return &bids_.begin()->second.front();
    }

    const Order *OrderBook::getBestAskOrder() const
    {
        if (asks_.empty())
        {
            return nullptr;
        }

        return &asks_.begin()->second.front();
    }

    const InstrumentId &OrderBook::getInstrumentId() const
    {
        return instrumentId_;
    }

    std::size_t OrderBook::getOrderCount() const
    {
        return orderIndex_.size();
    }

    bool OrderBook::empty() const
    {
        return orderIndex_.empty();
    }
}
