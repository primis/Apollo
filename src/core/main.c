/*
 * (c) 2016 Apollo Project Developers
 * Main.c - Main function of kernel activity
 */
#include <sys/init.h>
#include <sys/console.h>
#include <arch/x86/idt.h>
#include <sys/multiboot.h>

int main(multiboot_info_t* mbt)
{
    archInit();
    console_write("\nArch Enabled. Initializing Base System...\n");
    console_write("Multiboot header located at ");
    console_write_hex((unsigned int)&mbt);
    console_write(". flags are defined as: ");
    console_write_hex(mbt->flags);
    console_write(".\n");
    asm volatile ("int 0x03");
    return 0xDEADC0DE;
}
