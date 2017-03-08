/*
 * (c) 2016 Apollo Project Developers
 * Main.c - Main function of kernel activity
 */
#include <sys/init.h>
#include <sys/console.h>
#include <arch/x86/idt.h>
int main()
{
    archInit();
    console_write("Arch Enabled. Initializing Base System...\n");
    asm volatile ("int 0x03");
    return 0xDEADC0DE;
}
