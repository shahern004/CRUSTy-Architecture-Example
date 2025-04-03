# Embedded Migration Context: CRUSTy Architecture

This document tracks the progress and key details for migrating the CRUSTy Architecture project to an embedded target.

## Target Environment

- **Processor:** K65 Security Processor
- **FPGA:** Kintex Ultrascale+
- **Toolchain:** GNU arm-none-eabi
- **CPU Core:** Cortex-M4
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
  - Configured `.cargo/config.toml` for the target, initially specifying `arm-none-eabi-gcc` as the linker and a placeholder linker script (`-C link-arg=-Tlink.x`).
  - Ensured Rust crate (`src/lib.rs`) uses `#![no_std]` and includes necessary embedded crates (`cortex-m`, `cortex-m-rt`, `panic-halt`).

- **IAR Toolchain Detour:**

  - Briefly configured `.cargo/config.toml` to use the IAR linker (`ilinkarm.exe`) and a placeholder IAR linker script (`k65_linker_placeholder.icf`).
  - Build attempts failed because `ilinkarm.exe` was not found in the system PATH.
  - Archived IAR-specific files (`k65_linker_placeholder.icf`, `IAR_BUILD_GUIDE.md`, `main_reference.rs`) into `docs/IAR Reference/` for future reference.

- **Return to GNU Toolchain:**

  - Reverted `.cargo/config.toml` to use `arm-none-eabi-gcc` as the linker.
  - Created a placeholder GNU linker script `k65_linker_gnu_placeholder.ld` in the project root.
  - Updated `rustflags` in `.cargo/config.toml` to use the GNU linker script: `rustflags = ["-C", "link-arg=-Tk65_linker_gnu_placeholder.ld"]`.

- **C++ Cross-Compilation:**

  - Added `cc` crate as a build dependency in `Cargo.toml`.
  - Modified `build.rs` to conditionally compile C++ code (`src/fifo_cpp.cpp`) using `arm-none-eabi-g++` when the target is `thumbv7em-none-eabi`. Includes necessary flags (`-mcpu=cortex-m4`, `-mthumb`) and include paths.
  - Verified C++ compilation by running `cargo build --target thumbv7em-none-eabi`, which successfully created `libfifo_cpp.a`.

- **Rust Binary & Entry Point:**

  - Confirmed `cortex-m-rt` dependency exists in `Cargo.toml`.
  - Confirmed `#[entry]` function exists in `src/lib.rs` (later moved to `src/main.rs` when switching to binary crate type).
  - Initial builds with `crate-type = ["staticlib"]` succeeded but did not invoke the linker.
  - Switched to `crate-type = ["bin"]` and used `src/main.rs` with `#[entry]` to force linking.

- **Linker Script & Errors:**

  - Corrected a syntax error found in the placeholder `k65_linker_gnu_placeholder.ld`.
  - Encountered linker errors: `undefined reference to __libc_init_array` and `undefined reference to exit`. These indicated the linker was implicitly trying to link standard C library startup files (`crt0.o`).
  - Resolved these errors by adding the `-nostartfiles` linker flag to `rustflags` in `.cargo/config.toml`: `rustflags = ["-C", "link-arg=-Tk65_linker_gnu_placeholder.ld", "-C", "link-arg=-nostartfiles"]`.

- **Build Verification & Current Status:**
  - `cargo build --target thumbv7em-none-eabi` now completes successfully without linker errors.
  - Attempts to get detailed linker command/output using verbose flags (`-v`, `-vv`) or linker arguments (`-C link-arg=--verbose`) were unsuccessful; the exact linker invocation by `cargo`/`rustc` remains hidden.
  - The `--verbose` linker flag was reverted as it didn't provide the desired output.
  - **Current State:** The project successfully cross-compiles Rust and C++ code for `thumbv7em-none-eabi` using the GNU toolchain, linking them together without standard library startup files. Further testing requires a more complete linker script and potentially running on target hardware or a simulator. Focus has shifted temporarily to documentation consolidation.
