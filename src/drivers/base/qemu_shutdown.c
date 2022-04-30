/*
 * (c) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * qemu_shutdown.c - Driver for the x86 emulator qemu to allow quick shutdown
 */

#include <arch/x86/ports.h>
#include <sys/hal.h>
#include <stdint.h>

void system_shutdown()
{
    // First try older version (Qemu version older than 2.0)
    outw(0xB004, 0x2000);
    // Next try newer versions
    outw(0x604, 0x2000);
}
