/*
 * (c) 2016 Apollo Project Developers
 * Main.c - Main function of kernel activity
 */
#include <sys/init.h>
#include <sys/console.h>
#include <arch/x86/idt.h>
#include <sys/multiboot.h>

int main(multiboot_info_t* mbt, uint32_t magic)
{
    // Memory map for later use.
    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mbt->mmap_addr;

    archInit();
    console_write_hex(magic);
    console_write("\nArch Enabled. Initializing Base System...\n");
    while((uint32_t)mmap < mbt->mmap_addr + mbt->mmap_length) {
        mmap = (multiboot_memory_map_t*) ((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }
    console_write("Found Memory, KBytes: ");
    console_write_hex(mmap->addr);
    asm volatile ("int 0x03");
    return 0xDEADC0DE;
}
