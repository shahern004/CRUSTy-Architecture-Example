//! C-compatible API for CRUSTy Architecture
//! 
//! This module defines the FFI boundary between Rust and C/C++
//! for controlling the Kintex Ultrascale+ FPGA from IAR-compiled code.

#![no_std]

// use core::ptr;
// use volatile_register::{RW, RO};


/// Simple test function to verify FFI boundary is working
#[unsafe(no_mangle)]
pub unsafe extern "C" fn rust_function_blank() -> u32 {
    42
}
