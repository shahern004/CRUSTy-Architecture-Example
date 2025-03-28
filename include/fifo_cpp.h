#ifndef FIFO_CPP_H
#define FIFO_CPP_H

#include "CommandMessage.h" // Assuming you have a CommandMessage struct

void init_fifo_cpp();
CommandMessage read_fifo_cpp();
void simulate_interrupt_cpp(const CommandMessage& msg);

#endif
