/*
 * (c) 2018 Apollo Project Developers
 * arch/x86/init.c - Initialization function for x86 platform.
 */

#include <arch/x86/gdt.h>
#include <arch/x86/idt.h>
#include <arch/x86/vga.h>
#include <sys/console.h>
#include <sys/multiboot.h>


// We have to call main function from archInit.
void main();

/*
 * archInit() - Called by start.s to set up the hardware specific parts of
 * the kernel before calling main.
 */

int archInit(multiboot_info_t* mbt, unsigned int magic)
{
    consoleInit();
    gdtInit();
    idtInit();


    console_write("Checking for multiboot... ");

    if(magic == MULTIBOOT_BOOTLOADER_MAGIC) {   // Were we loaded by multiboot?
        console_write("[ OK ]\n");
    } else {
        console_write(" [ ERROR! ]\n");
        console_write(" Multiboot bootloader not detected, system halting!\n");
        return -1;
    }
    console_write("VBE reports address ");
    console_write_hex(mbt->framebuffer_addr & 0xFFFFFFFF);
    // We're all set up on the arch side, time to pass control to kernel main
    main();
    return 0;
}

