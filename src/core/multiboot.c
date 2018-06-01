/*
 * (c) 201 Apollo Project Developers
 * multiboot.c - Multiboot compliant bootloader specific parsing
 * This allows us to get as much information out of the bootloader as possible
 */

#include <sys/multiboot.h>
#include <stdio.h>
void multiboot_parse(multiboot_info_t *mbt)
{
    printf("Parsing multiboot information...\n");
    if(mbt->flags & MULTIBOOT_INFO_MEMORY) {
        printf("Memory information availible\n");
    }
    if(mbt->flags & MULTIBOOT_INFO_BOOTDEV) {
        printf("Boot device information availible\n");
    }
    if(mbt->flags & MULTIBOOT_INFO_CMDLINE) {
        printf("Kernel paramaters Availible\n");
    }
    if(mbt->flags & MULTIBOOT_INFO_MODS) {
        printf("Kernel modules loaded at boot time\n");
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
