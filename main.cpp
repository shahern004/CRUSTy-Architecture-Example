//#include <iostream.h>
#include <stdio.h>
#include "CrustyArch.h"

//Build Just this file iccarm.exe main.cpp -o mainIAR.o --eec++

int main() {
    uint32_t result = crusty::rust_function_blank();
    printf("Result from crusty::rust_function_blank() -> u32: %d\n", result);
    result = crusty::rust_function_blank();
    printf("Result from crusty::rust_function_blank() -> u32: %d\n", result);
    result = crusty::rust_function_blank();
    printf("Result from crusty::rust_function_blank() -> u32: %d\n", result);
    return 0;
}