/*
 * (c) 2016 Apollo Project Developers
 * arch/x86/init.c - Initialization function for x86 platform.
 */

#include <arch/x86/gdt.h>

/*
 * archInit() - Called by main to set up the hardware specific parts of
 * the kernel.
 */

void archInit()
{
    gdtInit();
}
