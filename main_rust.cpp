#include <iostream>
#include "include/crustyArchitecture.hpp"

int main() {
    auto* fifo_pair = init_fifo(); // Try without namespace if needed

    // Simulate interrupt events
    for (int i = 0; i < 5; ++i) {
        uint8_t data[64];
        for (int j = 0; j < 64; ++j) {
            data[j] = (uint8_t)(i * 10 + j);
        }
        simulate_interrupt(fifo_pair, i, data, sizeof(data));
    }

    for (int i = 0; i < 5; ++i) {
        CommandMessage msg = read_fifo(fifo_pair);
        if (msg.command_id >= 0) {
            std::cout << "Rust FIFO: Command ID: " << msg.command_id << std::endl;
        } else if (msg.command_id == -1) {
            std::cerr << "FIFO is empty" << std::endl;
        } else {
            std::cerr << "FIFO is disconnected" << std::endl;
        }
    }
    
    destroy_fifo(fifo_pair);
    return 0;
}