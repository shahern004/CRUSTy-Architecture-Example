//#include <iostream.h>
#include <stdio.h>
#include "include/CrustyArch.hpp"

//Build Just this file iccarm.exe main.cpp -o mainIAR.o --eec++

int main() {
    uint32_t result = crusty::rust_function_blank();
    printf("Result from rust_function_blank() -> u32: %d\n", result);
    result = crusty::rust_function_blank();
    printf("Result from rust_function_blank() -> u32: %d\n", result);
    result = crusty::rust_function_blank();
    printf("Result from rust_function_blank() -> u32: %d\n", result);
    return 0;
}