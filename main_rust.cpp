#include <iostream>
#include <cstdint> // Include for uintptr_t and uint8_t
#include "include/crustyArchitecture.hpp" // Will be regenerated

int main() {
    std::cout << "C++: Program Start." << std::endl << std::flush;

    std::cout << "C++: Calling init_fifo..." << std::endl << std::flush;
    // Use void* for the handle type again
    void* fifo_handle = crustyArchitecture::init_fifo();
    std::cout << "C++: init_fifo returned handle: " << fifo_handle << std::endl << std::flush;

    if (fifo_handle == nullptr) {
        std::cerr << "C++ Error: init_fifo returned null handle!" << std::endl << std::flush;
        return 1; // Exit if handle is null
    }

    std::cout << "C++: Starting send loop..." << std::endl << std::flush;
    // Simulate interrupt events
    for (int i = 0; i < 5; ++i) {
        std::cout << "C++: Preparing message " << i << "..." << std::endl << std::flush;
        uint8_t data[64];
        for (int j = 0; j < 64; ++j) {
            data[j] = static_cast<uint8_t>(i * 10 + j);
        }
        std::cout << "C++: Calling simulate_interrupt for ID " << i << "..." << std::endl << std::flush;
        // Use the namespace for the function call and check the return status
        int32_t send_status = crustyArchitecture::simulate_interrupt(fifo_handle, i, data, sizeof(data));
         std::cout << "C++: simulate_interrupt returned status " << send_status << std::endl << std::flush;
        if (send_status != 0) {
            std::cerr << "C++ Error: Failed to send message with command_id " << i << " (Status: " << send_status << ")" << std::endl << std::flush;
            // Consider breaking or handling the error more robustly
        }
    }
    std::cout << "C++: Send loop finished." << std::endl << std::flush;


    std::cout << "C++: Starting read loop..." << std::endl << std::flush;
    for (int i = 0; i < 5; ++i) {
         std::cout << "C++: Calling read_fifo (attempt " << i << ")..." << std::endl << std::flush;
        // Use the namespace for the type and function call
        crustyArchitecture::CommandMessage msg = crustyArchitecture::read_fifo(fifo_handle);
         std::cout << "C++: read_fifo returned msg with ID: " << msg.command_id << std::endl << std::flush;

        if (msg.command_id >= 0) {
            // This is the key output we want to see eventually
            std::cout << "  Rust FIFO: Command ID: " << msg.command_id << std::endl << std::flush;
        } else if (msg.command_id == -1) {
            std::cerr << "  Rust FIFO is empty" << std::endl << std::flush;
        } else if (msg.command_id == -2) {
             std::cerr << "  Rust FIFO is disconnected" << std::endl << std::flush;
        } else if (msg.command_id == -3) {
             std::cerr << "  C++ Error: read_fifo reported invalid handle!" << std::endl << std::flush;
        } else {
             std::cerr << "  C++ Error: read_fifo returned unexpected status: " << msg.command_id << std::endl << std::flush;
        }
    }
     std::cout << "C++: Read loop finished." << std::endl << std::flush;


    std::cout << "C++: Calling destroy_fifo..." << std::endl << std::flush;
    // Use the namespace for the function call
    crustyArchitecture::destroy_fifo(fifo_handle);
    std::cout << "C++: destroy_fifo returned." << std::endl << std::flush;

    std::cout << "C++: Exiting normally." << std::endl << std::flush;
    return 0;
}
