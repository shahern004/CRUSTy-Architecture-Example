# CRUSTy Architecture Setup Procedure

## Requirements for MY_RUST_LIB Integration

1.  **Install Rust and Cargo**  
    Download from https://www.rust-lang.org/tools/install

    rustup default stable
    rustup component add rustfmt

2.  **Install Rust and Cargo**
    cargo install --force cbindgen

3.  **Configure MinGW-W64 with specific compiler path**

    1. Download MinGW-W64 from: https://github.com/niXman/mingw-builds-binaries/releases/download/13.2.0-rt_v11-rev0/x86_64-13.2.0-release-win32-seh-ucrt-rt_v11-rev0.7z
    2. Extract to a location such as C:\mingw64
    3. Confirm that x86_64-w64-mingw32-g++ is in your PATH:
       where x86_64-w64-mingw32-g++
    4. If not found, add to PATH:
       set PATH=%PATH%;C:\path\to\mingw-w64\bin
    5. Verify the compiler is working:
       x86_64-w64-mingw32-g++ --version

4.  **Install ARM GNU Toolchain (for Cross-Compilation)**

    1. Download the appropriate GNU Arm Embedded Toolchain from ARM's website:
       https://developer.arm.com/downloads/-/gnu-rm
    2. Extract the toolchain to a known location (e.g., `C:\gcc-arm-none-eabi`).
    3. Add the toolchain's `bin` directory to your system PATH:
       `set PATH=%PATH%;C:\path\to\gcc-arm-none-eabi\bin`
    4. Verify the installation:
       `arm-none-eabi-gcc --version`

5.  **Install Rust Target (for Cross-Compilation)**

    - Install the required target triple for the Cortex-M4 core:
      `rustup target add thumbv7em-none-eabi`

6.  **Create project directory structure**

    mkdir crustyArchitectureExample
    cd crustyArchitectureExample
    mkdir src include

7.  Initialize Rust Library
    cargo init --lib

8.  **Modify Rust library code in src/lib.rs**

    - Open the file src/lib.rs and replace ALL existing content with:

    #[unsafe(no_mangle)]
    pub extern "C" fn rust_function_blank() -> u32 {
    42
    }

9.  **Create cbindgen.toml in project root**

    language = "C++"
    namespace = "crusty"
    include_guard = "CRUSTY_ARCHITECTURE_EXAMPLE_HPP"

10. **Create build.rs in project root**

    use std::env;
    use std::path::PathBuf;

    fn main() {
    let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let config = cbindgen::Config::from_file("cbindgen.toml").unwrap_or_default();

        let output_file = PathBuf::from(crate_dir.clone())
            .join("include")
            .join("CrustyArchitectureExample.hpp");

        cbindgen::Builder::new()
            .with_crate(crate_dir)
            .with_config(config)
            .generate()
            .expect("Unable to generate bindings")
            .write_to_file(output_file);

    }

11. **Update Cargo.toml**

    [package]
    name = "crustyArchitectureExample"
    version = "0.1.0"
    edition = "2021"

    [lib]
    name = "crustyArchitectureExample"
    crate-type = ["staticlib", "cdylib"]

    [build-dependencies]
    cbindgen = "0.24.3"

    # NOTE: Check cbindgen version first with 'cbindgen --version' and enter that version here.

12. **Create C test application in main.c**
    #include <iostream>
    #include "include/CrustyArchitectureExample.hpp"

    int main() {
    uint32_t result = crusty::rust_function_blank();
    std::cout << "Result from rust_function_blank() -> u32: " << result << std::endl;
    return 0;
    }

13. **Create Makefile**

    - Create a file named exactly Makefile (with capital M, no extension) in the project root:

    CXX = x86_64-w64-mingw32-g++
    CXXFLAGS = -Wall -std=c++17 -Iinclude
    LDFLAGS = -static

    all: prep main.exe

    prep:
    cargo build

    main.exe: main.cpp
    $(CXX) $(CXXFLAGS) -o $@ $< -L./target/debug -lcrustyArchitectureExample $(LDFLAGS)

    clean:
    cargo clean
    rm -f main.exe

    .PHONY: all prep clean

14. **Verify project structure**

    - Your project should have the following structure:
      crustyArchitectureExample/
      ├── Cargo.toml
      ├── Makefile
      ├── build.rs
      ├── cbindgen.toml
      ├── include/
      ├── main.cpp
      └── src/
      └── lib.rs

15. **Build the project**

    cargo build | mingw32-make

16. **Run the application**

    main.exe

17. **Verify correct output**
    - The output should read:
      'Result from rust_function_blank() -> u32: 42'
