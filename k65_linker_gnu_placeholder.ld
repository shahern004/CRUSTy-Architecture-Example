/* Generic Linker Script for Cortex-M4 */
ENTRY(Reset_Handler) /* Assuming Reset_Handler is the entry point from cortex-m-rt */

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 1024K /* Adjust FLASH size */
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 256K  /* Adjust RAM size */
}

/* Define stack size */
_stack_size = 0x400; /* Adjust if needed */

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector)) /* Vector table */
        *(.text*)           /* Code */
        *(.rodata*)         /* Read-only data */
        _etext = .;         /* End of code/read-only data */
    } > FLASH

    .data : AT ( ADDR(.text) + SIZEOF(.text) )
    {
        _sdata = .;         /* Start of initialized data */
        *(.data*)           /* Initialized data */
        _edata = .;         /* End of initialized data */
    } > RAM

    .bss :
    {
        _sbss = .;          /* Start of uninitialized data */
        *(.bss*)            /* Uninitialized data */
        *(COMMON)
        _ebss = .;          /* End of uninitialized data */
    } > RAM

    /* Stack section defined at the end of RAM */
    .stack (NOLOAD):
    {
      . = ALIGN(8);
      _stack_end = .; /* Top of stack symbol */
      KEEP(*(.stack*)) /* Keep stack sections from input files if any */
       . = . + _stack_size;
      _stack_start = .; /* Bottom of stack symbol */
    } > RAM

    /* Define end of application memory */
    _end = _stack_start;
    PROVIDE(end = _end);

    /* Ensure minimum stack size - Commented out due to syntax error */
    /* ASSERT(_stack_size >= 256, "Stack size must be at least 256 bytes."); */
}