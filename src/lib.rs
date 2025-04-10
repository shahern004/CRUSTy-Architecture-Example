#![no_std]

// Halt on panic
use panic_halt as _;


pub mod fifo;
pub use fifo::*;

// Include the content of rust_functions.rs directly or ensure it's properly included via mod
// Assuming rust_functions.rs defines items within a module, it should be declared here if needed.
// If rust_functions.rs is meant to be part of the crate root, its content might need merging
// or it should be declared as a module: pub mod rust_functions;
// Based on the read content of rust_functions.rs, it defines items inside `mod rust_functions { ... }`
// which is unusual. It should likely be `pub mod rust_functions;` here and the file content
// should not be wrapped in `mod rust_functions { ... }`.
// For now, I will add the module declaration as it seems intended.
pub mod rust_functions;
