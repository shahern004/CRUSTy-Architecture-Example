#ifndef CRUSTY_ARCHITECTURE_HPP
#define CRUSTY_ARCHITECTURE_HPP

#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

namespace crustyArchitecture {

struct CommandMessage {
  int32_t command_id;
  uint8_t data[64];
};

extern "C" {

void *init_fifo();

CommandMessage read_fifo(void *fifo_handle);

int32_t simulate_interrupt(void *fifo_handle,
                           int32_t command_id,
                           const uint8_t *data,
                           uintptr_t data_len);

void destroy_fifo(void *fifo_handle);

}  // extern "C"

}  // namespace crustyArchitecture

#endif  // CRUSTY_ARCHITECTURE_HPP
