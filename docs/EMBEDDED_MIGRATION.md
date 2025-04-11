# Embedded Migration Context: CRUSTy Architecture

This document tracks the progress and key details for migrating the CRUSTy Architecture project to an embedded target.

## Target Environment

- **Processor:** Kinetis K65 Security Processor
- **FPGA:** Kintex Ultrascale+
- **Toolchain:** IAR 
- **CPU Core:** Cortex-M4F
- **Rust Target Triple:** thumbv7em-none-eabi
- **Build Environment:** `no_std`

## Goals

1.  Successfully cross-compile the Rust crate for the target.
2.  Successfully cross-compile the C++ components for the target.
3.  Link Rust and C++ components into a single ELF executable for the target.
4.  Run a minimal application on the target (or simulator).
5.  Implement and test FPGA interaction logic.
6.  Ensure original functionality is preserved in the embedded environment.

## Key Files & Configuration

- `Cargo.toml`
- `src/lib.rs` (or `src/main.rs` if building binary)
- `.cargo/config.toml` (for target configuration)
- Linker Script: `k65_linker_gnu_placeholder.ld` (placeholder)
- `build.rs` (for C++ build and cbindgen)
- `Makefile` (for overall build orchestration, if used)
- `include/` (FFI headers)
- `main_cpp.cpp` / `main_rust.cpp` (Entry points/examples)

## Potential Issues (from Peer Experience)

- `Warning[Li008]: unsupported section flag 0x80000000`
- `Warning[Li026]: ... do not contain information to support Virtual Function Elimination`
- `Internal error: [CoreUtil/General]: Access violation (0xc0000005)`

## Progress Log (Consolidated from Memory Bank - 2025-04-03)

- **Initial Setup:**

  - Installed `thumbv7em-none-eabi` target via `rustup target add thumbv7em-none-eabi`.
  - Ensured Rust crate (`src/lib.rs`) uses `#![no_std]` and includes necessary embedded crates (`cortex-m`, `cortex-m-rt`, `panic-halt`).

- **IAR Toolchain Detour:**

  - Briefly configured `.cargo/config.toml` to use the IAR linker (`ilinkarm.exe`) and a placeholder IAR linker script (`k65_linker_placeholder.icf`).
  - Build attempts failed because `ilinkarm.exe` was not found in the system PATH.
  - Archived IAR-specific files (`k65_linker_placeholder.icf`, `IAR_BUILD_GUIDE.md`, `main_reference.rs`) into `docs/IAR Reference/` for future reference.

