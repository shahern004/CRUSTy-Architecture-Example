#![no_std]

// Halt on panic
use panic_halt as _;

pub mod fifo;
pub use fifo::*;
