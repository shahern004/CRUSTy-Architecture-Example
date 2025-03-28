#include <iostream>
#include "include/crustyArchitecture.hpp"

int main() {
    auto* fifi_pair = crustyArchitecture::init_fifo(); // Initialize the Rust FIFO (This function needs to be implemented in your Rust library)

    // Simulate interrupt events (replace with actual interrupt handling)
    for (int i = 0; i < 5; ++i) {
        uint8_t data[64];
        for (int j = 0; j < 64; ++j) {
            data[j] = (uint8_t)(i * 10 + j);
        }
        crustyArchitecture::simulate_interrupt(fifo_pair, i, data, sizeof(data));
    }

    for (int i = 0; i < 5; ++i) {
        auto result = crustyArchitecture::read_fifo(); // This function needs to be implemented in your Rust library
        if (result.is_ok()) {
            crustyArchitecture::CommandMessage msg = result.value();
            std::cout << "Rust FIFO: Command ID: " << msg.command_id << std::endl;
            // Process the data
        } else {
            std::cerr << "Error reading FIFO" << std::endl;
        }
    }
    crustyArchitecture::destroy_fifo(fifo_pair);
    return 0;
}
