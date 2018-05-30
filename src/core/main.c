/*
 * (c) 2016 Apollo Project Developers
 * Main.c - Main function of kernel activity
 */
#include <sys/init.h>
#include <sys/console.h>
#include <arch/x86/idt.h>
#include <sys/multiboot.h>

int main(multiboot_info_t* mbt, unsigned int magic)
{
    archInit();

    if(magic == MULTIBOOT_BOOTLOADER_MAGIC) {
        console_write("Confirmed Multiboot Compliant System\n");
        console_write("Flags from multiboot: ");
        console_write_hex(mbt->flags);
    }

    console_write("\nArch Enabled. Initializing Base System...\n");
    asm volatile ("int 0x03");
    return 0xDEADC0DE;
}
