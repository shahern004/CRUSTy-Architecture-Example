#include <stddef.h> // For size_t

/*
 * Low-level write function required by IAR DLIB library.
 * This is a stub implementation that does nothing but satisfy the linker.
 * It needs to be replaced with actual hardware output code (e.g., UART).
 *
 * Parameters:
 *  handle - File handle (1 for stdout, 2 for stderr)
 *  buf    - Pointer to the buffer with characters to write
 *  bufSize - Number of characters to write
 *
 * Returns:
 *  Number of characters written, or -1 on error.
 */
size_t __write(int handle, const unsigned char * buf, size_t bufSize)
{
  /* Remove this condition and implement actual output handling */
  if (buf == 0)
  {
    return 0; /* Avoid unused parameter warning */
  }

  /* Check for stdout/stderr */
  if (handle != 1 && handle != 2)
  {
    return (size_t)-1; /* Error */
  }

  /*
   * TODO: Implement character output for your target hardware here.
   * For example, loop through the buffer and send each character to UART.
   */

  /* Return the number of characters written */
  return bufSize;
}