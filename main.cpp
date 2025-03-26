#include <iostream>
#include "include/crustyArchitectureExample.hpp"

int main() {
    uint32_t result = crustyArchitecture::rust_function_blank();
    std::cout << "Result from rust_function_blank() -> u32: " << result << std::endl;
    return 0;
}