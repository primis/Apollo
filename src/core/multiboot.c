/*
 * (c) 201 Apollo Project Developers
 * multiboot.c - Multiboot compliant bootloader specific parsing
 * This allows us to get as much information out of the bootloader as possible
 */

#include <sys/multiboot.h>  // Provides multiboot headers
#include <sys/core.h>       // Provides kstalloc - Static allocator for early kernel use
#include <stdio.h>          // printf
#include <string.h>         // memcpy

// Destination, Source
void multiboot_parse(multiboot_info_t *mboot, multiboot_info_t *mbt)
{
    int len;
    printf("Parsing multiboot information...\n");

    // Copy the struct to a known location so we don't accidentally overwrite it.
    memcpy((unsigned char*)mboot, (unsigned char*)mbt, sizeof(multiboot_info_t));

    if(mbt->flags & MULTIBOOT_INFO_MEMORY) {
        printf("Memory information availible\n");
    }


    if(mbt->flags & MULTIBOOT_INFO_BOOTDEV) {
        printf("Boot device information availible\n");
    }

    if(mbt->flags & MULTIBOOT_INFO_CMDLINE) {
        printf("Kernel paramaters availible\n");
        len = strlen((char*)mbt->cmdline) + 1;
        mboot->cmdline = kstalloc(len);
        if (mboot->cmdline) {
            memcpy((unsigned char*)mboot->cmdline, (unsigned char*)mbt->cmdline, len);
        }
    }

    if(mbt->flags & MULTIBOOT_INFO_MODS) {
        printf("Kernel modules loaded at boot time\n");
        len = mbt->mods_count * sizeof(multiboot_module_t);
        mboot->mods_addr = kstalloc(len);
        if (mboot->mods_addr) {
            memcpy((unsigned char*)mboot->mods_addr, (unsigned char*)mbt->mods_addr, len);
        }
    }

    if(mbt->flags & MULTIBOOT_INFO_AOUT_SYMS) {
        printf("Kernel image symbol table availible\n");
    }

    if(mbt->flags & MULTIBOOT_INFO_ELF_SHDR) {
        printf("Kernel image ELF section header table availible\n");
    }

    if(mbt->flags & MULTIBOOT_INFO_MEM_MAP) {
        printf("System memory map availible\n");
    }

    if(mbt->flags & MULTIBOOT_INFO_DRIVE_INFO) {
        printf("System drive configuration availible\n");
    }

    if(mbt->flags & MULTIBOOT_INFO_CONFIG_TABLE) {
        printf("ROM Configuration table is accessable\n");
    }

    if(mbt->flags & MULTIBOOT_INFO_APM_TABLE) {
        printf("System APM table is accessable\n");
    }

    if(mbt->flags & MULTIBOOT_INFO_VBE_INFO) {
        printf("VBE control is availible\n");
    }

    if(mbt->flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO) {
        printf("Location of the framebuffer is availible\n");
    }
}
