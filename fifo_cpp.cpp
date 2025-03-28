#include "fifo_cpp.h" // Header file with function declarations
#include <iostream>
#include <vector> // Or other suitable data structures

// Simulated FIFO (replace with actual FPGA interaction if needed)
std::vector<CommandMessage> fifo;

void init_fifo_cpp() {
    // Initialize the simulated FIFO (if needed)
    fifo.reserve(100); // Example: Reserve space for 100 messages
}

CommandMessage read_fifo_cpp() {
    if (fifo.empty()) {
        // Handle FIFO empty condition (e.g., return an error code)
        CommandMessage empty_msg;
        empty_msg.command_id = -1; // Indicate error
        return empty_msg;
    }
    CommandMessage msg = fifo.front();
    fifo.erase(fifo.begin());
    return msg;
}

void simulate_interrupt_cpp(const CommandMessage& msg) {
    fifo.push_back(msg);
}
