# MarketFlow – Software Requirements Specification

**Project:** MarketFlow  
**Version:** 1.0  
**Language:** C++
**Status:** Initial design

## 1. Project Overview

MarketFlow is a C++20 financial market simulation engine designed to demonstrate order management, order-book data structures, price-time-priority matching, trade execution, and performance-oriented software engineering.

The system simulates a simplified financial exchange where participants submit buy and sell limit orders for financial instruments. The matching engine identifies compatible orders and executes trades according to predefined matching rules.

MarketFlow is an educational simulation and is not intended for real-world financial trading.

## 2. Project Objectives

- Implement a functional limit-order matching engine.
- Support multiple financial instruments with independent order books.
- Apply price-time-priority matching.
- Support partial and full order execution.
- Implement order cancellation and lifecycle management.
- Maintain trade execution history.
- Apply suitable data structures and algorithms for efficient order processing.
- Establish automated testing and performance measurement.

## 3. Scope

### 3.1 Included in the Initial MVP

- Buy and sell limit orders.
- Multiple financial instruments.
- Order validation.
- Price-time-priority matching.
- Partial and full execution.
- Order cancellation.
- Unique order identifiers.
- Order lifecycle tracking.
- Trade generation and in-memory trade history.
- Command-line interaction.
- Automated unit and integration testing.

### 3.2 Excluded from the Initial MVP

- Market orders and other advanced order types.
- Real-time external market data feeds.
- Live trading and brokerage integrations.
- Database persistence.
- Network communication and REST APIs.
- Kafka or external message brokers.
- Graphical user interfaces.
- Multithreaded order matching.

These features may be considered as future extensions after the core engine is stable.

## 4. Functional Requirements

| ID    | Requirement          | Description                                                       |
| ----- | -------------------- | ----------------------------------------------------------------- |
| FR-01 | Order Submission     | Accept buy and sell limit orders.                                 |
| FR-02 | Order Validation     | Validate order ID, instrument, price and quantity.                |
| FR-03 | Multiple Instruments | Maintain independent order books for each instrument.             |
| FR-04 | Order Prioritisation | Apply price-time priority.                                        |
| FR-05 | Order Matching       | Match compatible buy and sell orders.                             |
| FR-06 | Partial Execution    | Support execution of part of an order's remaining quantity.       |
| FR-07 | Full Execution       | Mark orders as filled when their remaining quantity reaches zero. |
| FR-08 | Order Cancellation   | Allow cancellation of active orders.                              |
| FR-09 | Order Lifecycle      | Maintain order statuses and executed quantities.                  |
| FR-10 | Trade Generation     | Generate a trade record for each successful match.                |
| FR-11 | Order Book Display   | Display outstanding buy and sell orders.                          |
| FR-12 | Trade History        | Maintain records of executed trades.                              |

## 5. Business Rules

### 5.1 Order Properties

Each order must contain:

- Unique order ID.
- Instrument identifier.
- Order side (BUY or SELL).
- Limit price.
- Original quantity.
- Remaining quantity.
- Executed quantity.
- Order status.
- Submission sequence number.

Prices will use integer ticks rather than floating-point values. The initial configuration will use one tick per cent.

For example, a price of $190.25 will be represented as 19025 ticks.

Quantities must be positive integers.

### 5.2 Order Matching Rules

The matching engine must apply the following rules:

**Buy order:**
A buy order can match the lowest-priced available sell order when:

`Best Ask Price <= Incoming Buy Limit Price`

**Sell order:**
A sell order can match the highest-priced available buy order when:

`Best Bid Price >= Incoming Sell Limit Price`

Matching continues until:

- The incoming order is completely filled, or
- No compatible resting order remains.

### 5.3 Price-Time Priority

Price priority:

- Buy orders: highest price first.
- Sell orders: lowest price first.

Time priority:

- Orders at the same price level are processed in submission order.
- A monotonically increasing sequence number will establish submission order.

### 5.4 Trade Execution Price

The execution price will be the resting order's price.

A resting order is an order already present in the order book when a new incoming order arrives.

For every execution:

`Trade Quantity = min(Incoming Remaining Quantity, Resting Remaining Quantity)`

Both orders' remaining quantities must be updated consistently.

### 5.5 Partial Execution

When only part of an order is executed:

- Its executed quantity increases.
- Its remaining quantity decreases.
- Its status becomes PARTIALLY_FILLED if a non-zero quantity remains.
- The remaining quantity stays eligible for future matching.

A completely executed order becomes FILLED.

### 5.6 Order Cancellation

- Only orders with a remaining quantity greater than zero can be cancelled.
- A cancelled order must be removed from the active order book.
- Previously executed quantities must be preserved.
- Cancellation must not reverse previously executed trades.
- A cancelled order must not participate in future matching.

### 5.7 Multiple Instruments

Each instrument must have its own independent order book.

An order for one instrument must never match an order for another instrument.

## 6. Order Lifecycle

The initial order statuses are:

- NEW
- OPEN
- PARTIALLY_FILLED
- FILLED
- CANCELLED
- REJECTED

Typical transitions:

- NEW → OPEN
- NEW → PARTIALLY_FILLED
- NEW → FILLED
- NEW → REJECTED
- OPEN → PARTIALLY_FILLED
- OPEN → FILLED
- OPEN → CANCELLED
- PARTIALLY_FILLED → FILLED
- PARTIALLY_FILLED → CANCELLED

A partially filled order may become CANCELLED if its remaining quantity is cancelled. Its previously executed quantity must remain recorded.

An incoming order that is not fully matched will have its unexecuted remainder inserted into the order book.

## 7. Non-Functional Requirements

### Performance

Use appropriate STL data structures and algorithms to avoid unnecessary order-book scans and inefficient matching operations.

### Correctness

- No order may execute more than its remaining quantity.
- No trade may occur at an incompatible price.
- Every trade must reference valid orders belonging to the same instrument.
- Order-book state must remain consistent after submission, execution and cancellation.

### Maintainability

Separate domain models, order management, order-book operations and matching logic into independent modules.

### Testability

Provide unit tests for individual components and integration tests for end-to-end matching scenarios.

### Reliability

Use appropriate validation, controlled state changes and clear error handling.

### Extensibility

Design the application so that persistence, networking, event processing and concurrent execution can be introduced later without unnecessarily coupling the core components.

## 8. Acceptance Criteria

The initial MVP will be considered functionally complete when:

1. Valid buy and sell limit orders can be submitted.
2. Invalid orders are rejected.
3. Multiple instruments have independent order books.
4. Orders are prioritised by price and then submission sequence.
5. Compatible orders are matched automatically.
6. Partial and full executions update quantities correctly.
7. Trade prices follow the resting-order-price rule.
8. Active orders can be cancelled.
9. Trade history is correctly maintained.
10. Automated tests cover matching, cancellation, validation and edge cases.
11. The application builds successfully using CMake and C++20.
12. All implemented automated tests pass.

## 9. Future Enhancements

After completing the MVP:

- Concurrent processing of independent instrument books.
- Database persistence using PostgreSQL.
- Network communication using Boost.Asio.
- Event-driven market data publication.
- Performance benchmarking and profiling.
- Additional order types and market simulation capabilities.

---
