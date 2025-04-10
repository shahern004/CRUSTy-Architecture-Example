# CRUST/y Architecture - Development Log

**Date:** 2025-03-28

This log documents significant changes and troubleshooting steps taken from the initial project baseline documented in `SystemDesignDocument.md` (Version 1.0).

## 1. cbindgen Header Generation Failure

- **Initial State:** `build.rs` was configured to run `cbindgen`, but the output header (`include/crustyArchitecture.hpp`) was empty. `main_rust.cpp` included this empty header but still linked (likely due to symbols being found in the static lib, but without type safety from the header).
- **Problem:** `cbindgen` failed to parse the Rust FFI function signatures, likely due to the complex pointer type used as a handle (`*mut Arc<Mutex<(Sender<CommandMessage>, Receiver<CommandMessage>)>>`). The `#[unsafe(no_mangle)]` attribute (required by the Rust 2024 compiler in this setup) was initially suspected but turned out not to be the root cause for `cbindgen`.
- **Change:** Modified `src/fifo.rs` FFI function signatures to use an opaque pointer (`*mut std::ffi::c_void`) as the handle type. Rust functions now cast this `void*` back to the internal `FifoHandleInner` type. Updated `main_rust.cpp` to use `void*` for the handle and call functions via the `crustyArchitecture::` namespace.
- **Result:** `cbindgen` successfully generated the C++ header with correct function signatures and the `CommandMessage` struct definition.
- **Impact:** Establishes a standard, robust FFI pattern using opaque pointers. C++ code remains unaware of Rust's internal types. Requires careful casting within Rust FFI functions.

## 2. C++ Linker Errors (`main_rust.exe`)

- **Initial State:** `Makefile` linked `main_rust.cpp` against `-lcrustyArchitecture` only.
- **Problem:** Numerous `undefined reference` errors occurred during linking, pointing to missing Windows API functions (Winsock, Userenv, NtApi) and C++/GCC runtime functions (`__chkstk`, `type_info`). This indicated the Rust static library (`libcrustyArchitecture.a`) relied on these system/runtime components, which weren't being linked into the final C++ executable.
- **Changes:**
  1.  Added necessary Windows libraries to the `main_rust.exe` link command: `-lws2_32 -luserenv -lntdll -ladvapi32 -lkernel32 -lmsvcrt`.
  2.  Added MinGW/GCC support libraries: `-lstdc++ -lgcc_s -lgcc -lmingwex`.
  3.  Removed `-static` from `LDFLAGS` to allow dynamic linking of C++ runtime.
  4.  Ensured Rust library was built for the target matching the C++ compiler (`--target x86_64-pc-windows-gnu` added to `cargo build`). This required installing the target via `rustup target add x86_64-pc-windows-gnu`.
  5.  Updated the linker library path to the target-specific output directory (`-L./target/x86_64-pc-windows-gnu/release`).
  6.  (Optional but applied) Set Rust panic behavior to `abort` in `Cargo.toml` (`[profile.release] panic = 'abort'`) to potentially simplify runtime interactions, though this wasn't the final fix for the linker errors.
- **Result:** Resolved all linker errors for `main_rust.exe`.
- **Impact:** The build process is now more complex, requiring specific Rust target installation and linking numerous libraries. The final executable `main_rust.exe` now depends on MinGW C++ runtime DLLs being available on the target system.

## 3. Build Failures (`main_cpp.exe` & `make clean`)

- **Problem 1:** `make` failed with `No rule to make target 'fifo_cpp.cpp'`.
- **Change 1:** Corrected the path in the `Makefile` dependency list for `main_cpp.exe` from `fifo_cpp.cpp` to `src/fifo_cpp.cpp`.
- **Problem 2:** C++ compilation failed for `main_cpp.cpp` and `src/fifo_cpp.cpp` due to unknown types `int32_t` and `uint8_t`.
- **Change 2:** Added `#include <cstdint>` to `include/CommandMessage.h` and `main_cpp.cpp`.
- **Problem 3:** `make clean` failed because `rm` command wasn't found on Windows.
- **Change 3:** Replaced `rm -f ...` with `del /f /q ... 2>nul || (exit 0)` in the `Makefile`'s `clean` target.
- **Result:** Resolved build errors for `main_cpp.exe` and ensured `make clean` works correctly on Windows.
- **Impact:** Build system is now more robust and platform-aware.

## 4. Troubleshooting `main_rust.exe` Runtime Failure

- **Initial State:** After refactoring `main_cpp.exe` to use `std::queue`, attempts were made to verify `main_rust.exe` execution before benchmarking.
- **Problem:** `main_rust.exe` failed to run, exiting immediately with error code 1 and producing no terminal output, even with added C++ and Rust print statements and flushing. This occurred despite successful compilation.
- **Troubleshooting Steps:**
  1.  **Return Status Check:** Modified Rust `simulate_interrupt` to return status codes; C++ checked these codes. No errors reported during send. Problem persisted.
  2.  **Handle Type Change:** Switched FFI handle from `*mut c_void` (using `Box::into_raw`/`from_raw`) to `usize` (using a static `HashMap` managed by `once_cell`). Problem persisted.
  3.  **Minimal FFI Test:** Simplified Rust library to a single `test_ffi_entry` function and C++ `main` to call only this function. Problem persisted.
  4.  **Dependency Check:** Used `objdump -p main_rust.exe` to inspect dynamic dependencies. Revealed it was linked against `crustyArchitecture.dll` instead of statically linking the `.a` library.
  5.  **Static Linking:** Modified `Cargo.toml` `crate-type` from `["staticlib", "cdylib"]` to `["staticlib"]`.
  6.  **Rebuild & Test:** Rebuilt the project. The minimal FFI test (`test_ffi_entry`) now ran successfully.
  7.  **Restore Logic:** Restored full FIFO logic to `src/fifo.rs` (using `*mut c_void` handles) and `main_rust.cpp`. Ensured `unsafe` blocks were correct in Rust FFI functions.
  8.  **Force Rebuild:** Explicitly deleted `main_rust.exe` and ran `mingw32-make main_rust.exe` to ensure recompilation.
- **Result:** `main_rust.exe` now runs correctly, executing the full FIFO logic and printing expected output (verified manually). The root cause was the build system incorrectly linking dynamically instead of statically.
- **Impact:** Static linking is crucial for this setup to work correctly. This significantly increases the executable size but resolves the runtime crash.

## 5. Final Benchmarking C++ vs. CRUSTy (Static Linking)

- **Initial State:** Both `main_cpp.exe` (using `std::queue`) and `main_rust.exe` (using Rust FIFO via FFI, statically linked) were confirmed to be working correctly and performing the same task (write 5, read 5 messages).
- **Benchmarking:**
  1.  Measured executable sizes using `dir *.exe`.
  2.  Measured execution speed using PowerShell's `Measure-Command { .\executable.exe }`, running each 5 times and calculating the average.
- **Result:**
  - **Size:**
    - `main_cpp.exe` (std::queue): **160,672 bytes** (Smaller)
    - `main_rust.exe` (CRUSTy, static): 5,408,483 bytes
  - **Speed (Avg. of 5 runs):**
    - `main_cpp.exe` (std::queue): 23.97 ms
    - `main_rust.exe` (CRUSTy, static): **15.87 ms** (Faster)
- **Impact:** With static linking, the CRUSTy executable is significantly larger due to including Rust's standard library components. However, it still demonstrates faster execution speed for this specific FIFO task compared to the standard C++ `std::queue` implementation. The trade-off between size and speed, along with build complexity, needs consideration for the target embedded environment.

## 6. CRUSTy Size Optimization (`no_std` Transition)

- **Initial State:** `main_rust.exe` (statically linked `std` Rust library) was ~5.4MB, significantly larger than the C++ baseline `main_cpp.exe` (~160KB). The goal was to reduce the CRUSTy executable size for embedded targets.
- **Problem:** The primary size contributor was the inclusion of Rust's standard library (`std`) via static linking.
- **Changes:**
  1.  **`no_std` Setup:** Modified `src/lib.rs` to add `#![no_std]` and `use panic_halt as _;`. Added `heapless` and `panic-halt` dependencies to `Cargo.toml`.
  2.  **Static FIFO Implementation:** Refactored `src/fifo.rs` to replace `std::sync::mpsc`, `Arc`, `Mutex`, and `Box` with a statically allocated `heapless::spsc::Queue` managed within a `spin::Mutex`. This involved:
      - Defining a `FifoState` struct holding `MaybeUninit` queue/producer/consumer and an `initialized` flag.
      - Using a `static FIFO_STATE: Mutex<FifoState>` to hold the single instance.
      - Updating FFI functions (`init`, `read`, `simulate`, `destroy`) to lock the mutex and operate on the state fields.
      - Replacing `std::slice` and `std::cmp` with `core::slice` and `core::cmp`.
      - Removing `println!` / `eprintln!` macros.
      - Resolving `static_mut_refs` errors by ensuring all access to static state happened through the locked mutex guard.
  3.  **Build Optimizations:**
      - Added `opt-level = "z"`, `lto = true`, `codegen-units = 1`, `strip = true` to `[profile.release]` in `Cargo.toml`.
      - Added `-Os` (optimize for size) to `CXXFLAGS` and `-s` (strip symbols) to `LDFLAGS` in the `Makefile`.
- **Result:**
  - The `no_std` Rust library (`libcrustyArchitecture.a`) compiled successfully.
  - The C++ application `main_rust.exe` linked and ran correctly using the `no_std` library.
  - Final executable sizes after optimizations:
    - `main_cpp.exe`: **19,456 bytes**
    - `main_rust.exe`: **19,968 bytes**
- **Impact:** The `no_std` transition combined with build optimizations dramatically reduced the `main_rust.exe` size, making it comparable to (and only ~500 bytes larger than) the optimized C++ baseline. This demonstrates the viability of the CRUSTy approach for size-constrained embedded environments by eliminating the Rust standard library overhead. The FFI interface remains the same from the C++ perspective.

## 7. Embedded Cross-Compilation Setup (thumbv7em-none-eabi)

- **Goal:** Migrate the build process from Windows/MinGW to the target embedded platform (`thumbv7em-none-eabi` using `arm-none-eabi-gcc`).
- **Initial Setup:**
  - Installed the Rust target: `rustup target add thumbv7em-none-eabi`.
  - Created/configured `.cargo/config.toml` for the target, specifying `linker = "arm-none-eabi-gcc"` and initial `rustflags` including a placeholder linker script (`-C link-arg=-Tlink.x`).
- **Toolchain Exploration (IAR Detour):**
  - Briefly attempted configuration using the IAR toolchain (`ilinkarm.exe`, `.icf` script). This failed due to the linker not being in the PATH and was abandoned in favor of the GNU toolchain.
  - IAR-related files were archived in `docs/IAR Reference/`.
- **GNU Toolchain Configuration:**
  - Reverted `.cargo/config.toml` to use `arm-none-eabi-gcc`.
  - Created a placeholder GNU linker script `k65_linker_gnu_placeholder.ld`.
  - Updated `rustflags` to point to this script: `rustflags = ["-C", "link-arg=-Tk65_linker_gnu_placeholder.ld"]`.
- **C++ Cross-Compilation (`build.rs`):**
  - Added `cc` crate as a build dependency.
  - Modified `build.rs` to detect the `thumbv7em-none-eabi` target and use `arm-none-eabi-g++` via the `cc` crate to compile C++ sources (`src/fifo_cpp.cpp`) into `libfifo_cpp.a`.
- **Rust Binary & Linking:**
  - Ensured `cortex-m-rt` dependency and `#[entry]` point were present.
  - Switched Rust crate type from `staticlib` to `bin` (using `src/main.rs`) to force linker invocation during `cargo build`.
- **Linker Issues & Resolution:**
  - Corrected syntax errors in the placeholder `k65_linker_gnu_placeholder.ld`.
  - Encountered linker errors for `__libc_init_array` and `exit`, indicating unwanted linking of standard C startup files.
  - Resolved by adding `-C link-arg=-nostartfiles` to `rustflags` in `.cargo/config.toml`.
- **Result:** The project now successfully cross-compiles both Rust and C++ code for the `thumbv7em-none-eabi` target using the GNU toolchain and links them into a binary without standard startup files. Build completes without errors.
- **Current Challenge:** Obtaining detailed linker command/output during the build process remains difficult, even with verbose flags.
