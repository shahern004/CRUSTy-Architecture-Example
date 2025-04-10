# CRUST/y Architecture Proof-of-Concept

**Version:** 1.1 (as of last major update documented in DESIGN.md)
**Date:** 2025-03-28 (as of last major update documented in DESIGN.md)

**1. Introduction**

This project demonstrates the CRUST/y (C++/Rust) architecture proof-of-concept (PoC). The primary goal of CRUST/y is to enable the use of Rust code within a C++ compiled environment, leveraging Rust's memory safety features for specific, potentially memory-sensitive operations, accessed via a Foreign Function Interface (FFI).

This PoC demonstrates the core concept using a simple FIFO (First-In, First-Out) queue implemented in Rust (`no_std`) and accessed from C++. It also includes a baseline pure C++ implementation for comparison.

**2. Core Interaction Pattern (CRUSTy Version)**

In the CRUSTy approach demonstrated here (`main_rust.exe`), the roles are clearly delineated:

- **Rust:** Manages the underlying FIFO queue mechanism, including data storage, internal state (empty/full), and thread safety. It handles the core queue logic and data transport _within_ the queue structure itself, exposing this functionality via the FFI.
- **C++:** Acts as the driver or consumer. It interacts with the Rust-managed queue through the defined FFI functions (`init_fifo`, `simulate_interrupt`, `read_fifo`, `destroy_fifo`). C++ sends commands/data _to_ the Rust queue and receives messages _from_ it. Once a message is retrieved from Rust, the C++ code is responsible for processing or acting upon that message.

_(See [DESIGN.md](./DESIGN.md) for a detailed architecture diagram)._

**3. Current Status**

The project is currently undergoing migration to target an embedded K65 processor (`thumbv7em-none-eabi`). See [EMBEDDED_MIGRATION.md](./EMBEDDED_MIGRATION.md) for details on this effort.

**4. Project Documentation**

For detailed information, please refer to the following documents:

- **[SETUP.md](./SETUP.md):** Instructions for setting up the original Windows/MinGW development environment.
- **[DESIGN.md](./DESIGN.md):** Detailed system architecture, build system, FFI interface, and component descriptions.
- **[DEVELOPMENT.md](./DEVELOPMENT.md):** Historical log of significant changes, troubleshooting, and design decisions during the PoC development.
- **[TESTING.md](./TESTING.md):** Benchmark methodology and results comparing the optimized C++ and `no_std` Rust implementations.
- **[EMBEDDED_MIGRATION.md](./EMBEDDED_MIGRATION.md):** Context, goals, and progress specific to the ongoing embedded target migration.
