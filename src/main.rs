#![no_std]
#![no_main]

use panic_halt as _; // you can put a breakpoint on `rust_begin_unwind` to catch panics
use cortex_m_rt::entry;

#[entry]
fn main() -> ! {
    // your code goes here

    loop {
        // Loop forever
    }
}