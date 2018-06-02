/*
 * (c) 2018 Apollo Project Developers
 * arch/x86/init.c - Initialization function for x86 platform.
 */

#include <stdio.h>
#include <sys/core.h>
#include <arch/x86/gdt.h>
#include <arch/x86/idt.h>
#include <arch/x86/vga.h>

/*
 * archInit() - Called by start.s to set up the hardware specific parts of
 * the kernel before calling main.
 */

multiboot_info_t *mboot;

int archInit(multiboot_info_t* mbt, unsigned int magic)
{

    consoleInit();
    gdtInit();
    idtInit();


    printf("Checking for multiboot... ");

    if(magic == MULTIBOOT_BOOTLOADER_MAGIC) {   // Were we loaded by multiboot?
        printf("[ OK ]\n");
    } else {
        printf(" [ ERROR! ]\n");
        printf(" Multiboot bootloader not detected, system halting!\n");
        return -1;
    }
    // Now time to parse the multiboot info (for debugging) and copy
    // the contents of it to known memory
    multiboot_parse(mboot, mbt);
    // We're all set up on the arch side, time to pass control to kernel main
    main();
    return 0;
}

