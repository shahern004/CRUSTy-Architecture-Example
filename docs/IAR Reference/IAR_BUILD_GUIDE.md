# Guide: Integrating IAR C++ Compilation into Rust Build (`build.rs`)

This guide outlines the steps required to modify the `build.rs` script in the CRUSTy Architecture project to compile C++ source files using the IAR C/C++ Compiler (`iccarm.exe`) for the `thumbv7em-none-eabi` target.

## Prerequisites

1.  **IAR Embedded Workbench for Arm:** Ensure you have a compatible version of IAR Embedded Workbench installed on your build machine.
2.  **Environment Setup (PATH):** The directory containing `iccarm.exe` (typically `C:\Program Files\IAR Systems\Embedded Workbench X.Y\arm\bin` or similar) **must** be added to your system's `PATH` environment variable. Cargo's build script (`build.rs`) relies on the `PATH` to locate the compiler executable. Verify this by opening a terminal and running `iccarm.exe --version`.

## Steps

1.  **Add `cc` Crate Dependency:**
    The `cc` crate provides a convenient way to invoke C/C++ compilers from `build.rs`. Add it to the `[build-dependencies]` section of your `Cargo.toml` file:

    ```toml
    [build-dependencies]
    cbindgen = "0.28.0"
    cc = "1.0" # Or the latest compatible version
    ```

2.  **Modify `build.rs`:**
    Update your `build.rs` file to include the logic for C++ compilation. The following example demonstrates how to compile `src/fifo_cpp.cpp`:

    ```rust
    use std::env;
    use std::path::PathBuf;

    fn main() {
        // --- Header Generation (Existing Code) ---
        let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
        let config = cbindgen::Config::from_file("cbindgen.toml").unwrap_or_default();

        let output_file = PathBuf::from(crate_dir.clone())
            .join("include")
            .join("crustyArchitecture.hpp");

        cbindgen::Builder::new()
            .with_crate(crate_dir)
            .with_config(config)
            .generate()
            .expect("Unable to generate bindings")
            .write_to_file(&output_file);

        println!("cargo:rerun-if-changed=cbindgen.toml");
        println!("cargo:rerun-if-changed=src/lib.rs");
        // Add other relevant Rust source files if needed
        println!("cargo:rerun-if-changed=src/rust_functions.rs");
        println!("cargo:rerun-if-changed=src/fifo.rs");


        // --- C++ Compilation for IAR ---
        let target = env::var("TARGET").unwrap();

        // Only compile C++ when targeting the specific embedded platform
        if target.starts_with("thumbv7em") {
            println!("cargo:rerun-if-changed=src/fifo_cpp.cpp");
            println!("cargo:rerun-if-changed=include/fifo_cpp.h"); // Rerun if header changes

            cc::Build::new()
                .compiler("iccarm.exe")     // Specify the IAR compiler executable
                .file("src/fifo_cpp.cpp") // The C++ source file to compile
                .include("include")       // Add the 'include' directory for headers like fifo_cpp.h
                .flag("--cpu")            // IAR flag to specify the target CPU core
                .flag("Cortex-M4")        // Set the core to Cortex-M4
                .flag("--endian=little")  // Specify little-endian byte order
                .flag("-Ohz")             // Example: Optimize for size (consult IAR docs for best flags)
                // Add any other necessary IAR-specific flags here.
                // Examples:
                // .flag("--debug")          // Enable debug information
                // .flag("--dlib_config")   // Specify DLib configuration if needed
                // .flag("C:\\path\\to\\iar\\config\\Normal\\MyDlibConfig.h")
                // .define("MY_PREPROCESSOR_MACRO", Some("value")) // Define preprocessor macros
                .cpp(true)                // Explicitly tell the cc crate this is C++ code
                .target(&target)          // Pass the target triple
                .host(&env::var("HOST").unwrap()) // Pass the host triple
                .opt_level(env::var("OPT_LEVEL").unwrap().parse().unwrap()) // Pass optimization level
                .compile("fifo_cpp");     // The base name for the output static library (libfifo_cpp.a)

            // Instruct Cargo to link against the compiled C++ static library
            println!("cargo:rustc-link-lib=static=fifo_cpp");
            // Tell Cargo where to find the compiled library (usually OUT_DIR)
            println!("cargo:rustc-link-search=native={}", env::var("OUT_DIR").unwrap());
        }
    }
    ```

    **Explanation of Key `cc::Build` Settings:**

    - `.compiler("iccarm.exe")`: Tells the `cc` crate to use the IAR compiler. This relies on `iccarm.exe` being in the system `PATH`. If it's not, you might need to provide the full absolute path.
    - `.file("src/fifo_cpp.cpp")`: Specifies the C++ source file. Add more `.file()` calls for additional C++ files.
    - `.include("include")`: Adds the `include` directory to the compiler's search path for header files (`#include <...>`).
    - `.flag("--cpu")`, `.flag("Cortex-M4")`: Passes the `--cpu Cortex-M4` argument to `iccarm.exe`. Add other necessary flags using `.flag()`. Consult the IAR C/C++ Development Guide for your specific device and requirements.
    - `.cpp(true)`: Ensures the `cc` crate treats the source file as C++ and uses the appropriate compiler invocation.
    - `.compile("fifo_cpp")`: Compiles the source file(s) into a static library named `libfifo_cpp.a` (the `lib` prefix and `.a` suffix are added automatically) placed in Cargo's `OUT_DIR`.
    - `println!("cargo:rustc-link-lib=static=fifo_cpp");`: Tells `rustc` to link against the static library `libfifo_cpp.a`.
    - `println!("cargo:rustc-link-search=native={}", env::var("OUT_DIR").unwrap());`: Tells `rustc` where to find the compiled C++ library.

3.  **Configure IAR Linker in `.cargo/config.toml`:**
    Ensure your `.cargo/config.toml` correctly specifies the IAR linker (`ilinkarm.exe`) for the target:

    ```toml
    [target.thumbv7em-none-eabi]
    linker = "ilinkarm.exe"
    # Add necessary rustflags for the linker, including the linker script
    rustflags = [
      "-C", "link-arg=--config",
      "-C", "link-arg=k65_linker_placeholder.icf", # Replace with your actual IAR linker script
      # Add other necessary IAR linker flags here
      # "-C", "link-arg=--semihosting", # Example
    ]
    ```

4.  **Build the Project:**
    Run the Cargo build command specifying the target:

    ```bash
    cargo build --target thumbv7em-none-eabi -v
    ```

    Using the `-v` (verbose) flag is helpful to see the exact commands being executed by `build.rs` and `rustc`, including the `iccarm.exe` invocation.

## Troubleshooting

- **`iccarm.exe` not found:** Double-check that the IAR `bin` directory is correctly added to your system `PATH` and that you can invoke `iccarm.exe` from a new terminal session. If issues persist, try specifying the full path in `build.rs`: `.compiler("C:\\path\\to\\iar\\arm\\bin\\iccarm.exe")`.
- **Compilation Errors:** Examine the errors reported by `iccarm.exe` in the build output. These are likely due to:
  - Missing or incorrect compiler flags (e.g., wrong `--cpu`, missing defines, incorrect include paths).
  - Syntax errors in the C++ code.
  - Incompatibilities between the C++ code and the target configuration. Consult the IAR documentation for flag details.
- **Linking Errors:** Errors during the final linking stage (after `build.rs` succeeds) are often related to the linker script (`.icf` file), missing symbols, or incorrect linker flags passed via `rustflags` in `config.toml`.

This guide provides the foundational steps. You may need to adjust compiler and linker flags based on your specific project requirements, C++ code complexity, and the target hardware configuration. Always refer to the IAR Embedded Workbench documentation for detailed information on compiler and linker options.
