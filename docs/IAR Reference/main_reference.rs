#![no_std]
#![no_main]

use core::panic::PanicInfo;

// Entry point for the application
#[cortex_m_rt::entry]
fn main() -> ! {
    // Minimal application logic (or none)
    loop {}
}

// Panic handler
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}