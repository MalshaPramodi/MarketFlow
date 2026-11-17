# MarketFlow

A C++20 financial market simulation engine designed to demonstrate
order management, price-time-priority matching, data structures,
and performance-oriented software engineering.

## Overview

MarketFlow is a modular financial market simulation project that
aims to implement a limit order matching engine.

The engine will support buy and sell orders, maintain separate
order books for financial instruments, and match compatible orders
according to price-time priority.

## Key Features

Planned features:

- Limit buy and sell orders
- Multiple financial instruments
- Price-time-priority order matching
- Partial and full order execution
- Order cancellation and lifecycle management
- Trade generation and history
- Automated unit and integration testing
- Performance benchmarking

## Technology Stack

| Technology | Purpose                        |
| ---------- | ------------------------------ |
| C++20      | Core implementation            |
| CMake      | Build configuration            |
| STL        | Data structures and algorithms |
| GoogleTest | Automated testing              |
| Git        | Version control                |

## Project Architecture

The project is being developed as a modular monolith,
with responsibilities separated into:

- Order Management
- Order Book
- Matching Engine
- Trade Execution
- Market Data Processing

## Project Structure

```text
MarketFlow/
├── include/
├── src/
├── tests/
├── benchmarks/
├── docs/
├── scripts/
├── CMakeLists.txt
├── README.md
├── .gitignore
└── LICENSE
```

## Building the Project

Build instructions will be added after the CMake configuration
and initial implementation are established.

## Testing

Automated tests will be implemented using GoogleTest.

## Project Status

Under active development.

## For your information

MarketFlow is an educational financial market simulation.
It is not intended for real-world trading or financial transactions.
