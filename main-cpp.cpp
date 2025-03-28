#include <iostream>
#include "fifo_cpp.h"

int main() {
    init_fifo_cpp(); // Initialize the C++ FIFO

    // Simulate interrupt events
    for (int i = 0; i < 5; ++i) {
        CommandMessage msg;
        msg.command_id = i;
        // Populate msg.data with some sample data
        for (int j = 0; j < 64; ++j) {
            msg.data[j] = (uint8_t)(i * 10 + j);
        }
        simulate_interrupt_cpp(msg);
    }

    // Read data from the FIFO
    for (int i = 0; i < 5; ++i) {
        CommandMessage msg = read_fifo_cpp();
        if (msg.command_id == -1) {
            std::cerr << "Error reading FIFO: FIFO is empty" << std::endl;
        } else if (msg.command_id == -2) {
            std::cerr << "Error reading FIFO: FIFO is disconnected" << std::endl;
        } else {
            std::cout << "C++ FIFO: Command ID: " << msg.command_id << std::endl;
            // Process msg.data here
        }
    }

    return 0;
}
