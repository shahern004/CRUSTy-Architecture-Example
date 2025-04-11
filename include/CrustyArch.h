#ifndef CRUSTY_ARCHITECTURE_EXAMPLE_H
#define CRUSTY_ARCHITECTURE_EXAMPLE_H

#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

namespace crusty {

extern "C" {

/// Simple test function to verify FFI boundary is working
uint32_t rust_function_blank();

}  // extern "C"

}  // namespace crusty

#endif  // CRUSTY_ARCHITECTURE_EXAMPLE_H
