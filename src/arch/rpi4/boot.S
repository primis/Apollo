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
    ldr     x5, =__bss_start// Starting Address
    ldr     w6, =__bss_size // Loop Counter (size in bytes of BSS)
1:  cbz     w6, 2f          // Quit loop if counter is 0
    str     xzr, [x5], #8   // Store 0 in address of x5, then add 8 bytes to x5
    sub     w6, w6, #1      // Decrement loop counter
    cbnz    w6, 1b          // Loop back around if counter isn't 0
2:  bl      arch_init       // Jump to C code
    b       1b              // In the event of return, halt
