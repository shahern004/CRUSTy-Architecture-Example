use std::env;
use std::path::PathBuf;

fn main() {
    // --- C++ Compilation ---
    let target = env::var("TARGET").unwrap();

    // Only compile C++ code when targeting thumbv7em-none-eabi
    if target == "thumbv7em-none-eabi" {
        println!("cargo:rerun-if-changed=src/fifo_cpp.cpp");
        println!("cargo:rerun-if-changed=include/fifo_cpp.h");

        cc::Build::new()
            .compiler("arm-none-eabi-g++") // Specify the cross-compiler
            .file("src/fifo_cpp.cpp")      // Specify the C++ source file
            .include("include")            // Specify the include directory
            .flag("-mcpu=cortex-m4")       // Set the CPU target
            .flag("-mthumb")               // Use Thumb instruction set
            // Add other necessary flags here, e.g., FPU flags if needed
            // .flag("-mfpu=fpv4-sp-d16") // Example FPU flag (check if needed)
            .cpp(true)                     // Ensure C++ mode is enabled
            .compile("fifo_cpp");          // Compile the code into libfifo_cpp.a
    }

    // --- Header Generation (cbindgen) ---
    let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let config = cbindgen::Config::from_file("cbindgen.toml").unwrap_or_default();

    let output_file = PathBuf::from(crate_dir.clone())
        .join("include")
        .join("crustyArchitecture.hpp");

    println!("cargo:rerun-if-changed=cbindgen.toml");
    println!("cargo:rerun-if-changed=src/lib.rs");
    // Add other Rust source files if they contribute to the C API
    println!("cargo:rerun-if-changed=src/rust_functions.rs");
    println!("cargo:rerun-if-changed=src/fifo.rs");


    cbindgen::Builder::new()
        .with_crate(crate_dir)
        .with_config(config)
        .generate()
        .expect("Unable to generate bindings")
        .write_to_file(output_file);
}