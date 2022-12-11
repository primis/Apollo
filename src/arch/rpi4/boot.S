/*
 * Copyright (c) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * Entry point for the rpi4 architecture port
 */

.section ".init"
.globl _start
// Entry Point for the kernel. The Pi provides the following:
// x0 -> 32 bit pointer to DTB in memory (primary core) / 0 (secondary cores)
// x1 -> 0
// x2 -> 0
// x3 -> 0
// x4 -> 32 bit kernel entry point (_start's address)

// With latest firmware, primary core runs this code, while all the secondairy
// cores sit in a spinloop.

_start:
    // Set up the stack below our code
    ldr x5, =_start
    mov sp, x5

    // Zero out the BSS section
    ldr     x5, =__bss_begin// Start Address of .bss segment
    ldr     x6, =__bss_end  // End Address of .bss segment
    sub     x6, x6, x5      // Size = start - end

1:  cbz     x6, 2f          // Quit loop if counter is 0
    str     xzr, [x5], #8   // Store 0 at address X5 , then add 8 bytes to x5
    sub     x6, x6, #1      // Decrement loop counter
    cbnz    x6, 1b          // Loop back around if counter isn't 0

2:  bl      arch_init       // Jump to C code
    b       1b              // In the event of return, halt
