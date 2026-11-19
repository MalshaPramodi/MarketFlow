#include "models/order.hpp"

#include <stdexcept>
#include <utility>

namespace marketflow
{
    Order::Order(
        OrderId orderId,
        InstrumentId instrumentId,
        OrderSide side,
        Price price,
        Quantity quantity,
        SequenceNumber sequenceNumber)
        : orderId_(std::move(orderId)),
          instrumentId_(std::move(instrumentId)),
          side_(side),
          price_(price),
          quantity_(quantity),
          remainingQuantity_(quantity),
          status_(OrderStatus::New),
          sequenceNumber_(sequenceNumber)
    {
        if (orderId_.empty())
        {
            throw std::invalid_argument("Order ID cannot be empty");
        }

        if (instrumentId_.empty())
        {
            throw std::invalid_argument("Instrument ID cannot be empty");
        }

        if (price_ <= 0)
        {
            throw std::invalid_argument("Order price must be positive");
        }

        if (quantity_ == 0)
        {
            throw std::invalid_argument("Order quantity must be positive");
        }
    }

    const OrderId &Order::getOrderId() const
    {
        return orderId_;
    }

    const InstrumentId &Order::getInstrumentId() const
    {
        return instrumentId_;
    }

    OrderSide Order::getSide() const
    {
        return side_;
    }

    Price Order::getPrice() const
    {
        return price_;
    }

    Quantity Order::getQuantity() const
    {
        return quantity_;
    }

    Quantity Order::getRemainingQuantity() const
    {
        return remainingQuantity_;
    }

    OrderStatus Order::getStatus() const
    {
        return status_;
    }

    SequenceNumber Order::getSequenceNumber() const
    {
        return sequenceNumber_;
    }

    void Order::markOpen()
    {
        if (status_ != OrderStatus::New)
        {
            throw std::logic_error("Only a new order can be marked open");
        }

        status_ = OrderStatus::Open;
    }

    void Order::applyFill(Quantity fillQuantity)
    {
        if (status_ != OrderStatus::New && !isActive())
        {
            throw std::logic_error("Cannot fill an inactive order");
        }

        if (fillQuantity == 0 || fillQuantity > remainingQuantity_)
        {
            throw std::invalid_argument("Invalid fill quantity");
        }

        remainingQuantity_ -= fillQuantity;

        status_ = (remainingQuantity_ == 0)
                      ? OrderStatus::Filled
                      : OrderStatus::PartiallyFilled;
    }

    void Order::cancel()
    {
        if (!isActive())
        {
            throw std::logic_error("Only an active order can be cancelled");
        }

        status_ = OrderStatus::Cancelled;
    }

    void Order::reject()
    {
        if (status_ != OrderStatus::New)
        {
            throw std::logic_error("Only a new order can be rejected");
        }

        status_ = OrderStatus::Rejected;
    }

    bool Order::isActive() const
    {
        return status_ == OrderStatus::Open ||
               status_ == OrderStatus::PartiallyFilled;
    }
}
