#include <iostream>
#include <cstdint> // Include for uint8_t
#include <queue>   // Include the standard queue
#include "include/CommandMessage.h" // Include the message definition

int main() {
    // Use std::queue to hold the messages
    std::queue<CommandMessage> message_queue;

    std::cout << "C++ (std::queue): Simulating writes..." << std::endl;
    // Simulate interrupt events by pushing to the queue
    for (int i = 0; i < 5; ++i) {
        CommandMessage msg;
        msg.command_id = i;
        // Populate msg.data with some sample data
        for (int j = 0; j < 64; ++j) {
            msg.data[j] = static_cast<uint8_t>(i * 10 + j); // Use static_cast for clarity
        }
        message_queue.push(msg); // Push the message onto the queue
    }

    std::cout << "C++ (std::queue): Reading back..." << std::endl;
    // Read data from the queue
    int messages_read = 0;
    while (!message_queue.empty() && messages_read < 5) { // Read up to 5 or until empty
        CommandMessage msg = message_queue.front(); // Get the front message
        message_queue.pop(); // Remove the message from the queue
        messages_read++;

        // Process the message (print command ID)
        std::cout << "C++ std::queue: Command ID: " << msg.command_id << std::endl;
        // Optionally print some data
        // std::cout << "  Data[0]: " << static_cast<int>(msg.data[0]) << std::endl;
    }

    if (messages_read == 0 && message_queue.empty()) {
         std::cerr << "C++ std::queue: Queue was empty." << std::endl;
    } else if (messages_read < 5) {
         std::cout << "C++ std::queue: Read " << messages_read << " messages (fewer than 5 written)." << std::endl;
    }


    // No explicit destroy needed for std::queue, it cleans up automatically

    return 0;
}
