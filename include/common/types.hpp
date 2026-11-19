#pragma once

#include <cstdint>
#include <string>

namespace marketflow
{
    using OrderId = std::string;
    using InstrumentId = std::string;

    using Price = std::int64_t;
    using Quantity = std::uint64_t;

    using SequenceNumber = std::uint64_t;
    using TradeId = std::uint64_t;

    enum class OrderSide
    {
        Buy,
        Sell
    };

    enum class OrderStatus
    {
        New,
        Open,
        PartiallyFilled,
        Filled,
        Cancelled,
        Rejected
    };
}
