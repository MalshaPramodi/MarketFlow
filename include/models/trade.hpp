#pragma once

#include "common/types.hpp"

namespace marketflow
{
    struct Trade
    {
        TradeId tradeId;
        InstrumentId instrumentId;

        OrderId makerOrderId;
        OrderId takerOrderId;

        Price price;
        Quantity quantity;

        SequenceNumber sequenceNumber;
    };
}
