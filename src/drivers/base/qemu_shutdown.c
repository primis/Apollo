/*
 * (c) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * qemu_shutdown.c - Driver for the x86 emulator qemu to allow quick shutdown
 */

#include <stdint.h>
#include <sys/resource.h>

static resource_t old_qemu = {
    .start = 0xB004,
    .end   = 0xB004,
    .flags = RESOURCE_IO
};

static resource_t new_qemu = {
    .start = 0x604,
    .end   = 0x604,
    .flags = RESOURCE_IO
};

void system_shutdown()
{
    uint16_t cmd = 0x2000;

    // First try older version (Qemu version older than 2.0)
    resource_write(&cmd, &old_qemu, 0, 1);
    // Next try newer versions
    resource_write(&cmd, &new_qemu, 0, 1);
}
