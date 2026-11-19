#pragma once

#include "common/types.hpp"

namespace marketflow
{
    class Order
    {
    public:
        Order(
            OrderId orderId,
            InstrumentId instrumentId,
            OrderSide side,
            Price price,
            Quantity quantity,
            SequenceNumber sequenceNumber);

        // Read-only accessors
        const OrderId &getOrderId() const;
        const InstrumentId &getInstrumentId() const;

        OrderSide getSide() const;
        Price getPrice() const;

        Quantity getQuantity() const;
        Quantity getRemainingQuantity() const;

        OrderStatus getStatus() const;
        SequenceNumber getSequenceNumber() const;

        // Order lifecycle operations
        void markOpen();
        void applyFill(Quantity fillQuantity);
        void cancel();
        void reject();

        bool isActive() const;

    private:
        OrderId orderId_;
        InstrumentId instrumentId_;

        OrderSide side_;
        Price price_;

        Quantity quantity_;
        Quantity remainingQuantity_;

        OrderStatus status_;
        SequenceNumber sequenceNumber_;
    };
}
