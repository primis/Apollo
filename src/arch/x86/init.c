/*
 * (c) 2019 Apollo Project Developers
 * For terms, see LICENSE
 * arch/x86/init.c - Initialization function for x86 platform.
 */

#include <types.h>
#include <sys/multiboot.h>
#include <string.h>

extern int main(int argc, char **argv);

multiboot_info_t mboot;

// 4k for early allocator - for multiboot and things
#define EARLYALLOC_SZ 4096

// Bump Allocator, used to store multiboot info to a safe location
static uintptr_t earlyalloc(unsigned len)
{
    static uint8_t buf[EARLYALLOC_SZ];
    static unsigned idx = 0;
    if (idx + len >= EARLYALLOC_SZ) {
        // We can't allocate, we ran outta room
        return NULL;
    }
    uint8_t *ptr = &buf[idx];
    idx += len;

    return (uintptr_t)ptr;
}

// Called by assembly.
int arch_init(multiboot_info_t* mbt, unsigned int magic)
{
    int i, len;
    // Be nice to c++
    extern size_t __ctors_begin;
    extern size_t __ctors_end;
    for (size_t *i = &__ctors_begin; i< &__ctors_end; i++) {
        ((void (*)(void)) *i)();
    }

    // Copy over multiboot data to new struct.
    memcpy((uint8_t*)&mboot, (uint8_t*)mbt, sizeof(multiboot_info_t));
    // Now copy info inside to malloc()'d space

    // Command line first
    if (mboot.flags & MULTIBOOT_INFO_CMDLINE) {
        len = strlen((char*)mbt->cmdline) + 1;
        mboot.cmdline = earlyalloc(len);
        if (mboot.cmdline) {
            memcpy((uint8_t*)mboot.cmdline, (uint8_t*)mbt->cmdline, len);
        }
    }
    if (mboot.flags & MULTIBOOT_INFO_MODS) {
        len = mboot.mods_count * sizeof(multiboot_module_t);
        mboot.mods_addr = earlyalloc(len);
        if(mboot.mods_addr) {
            memcpy((uint8_t*)mboot.mods_addr, (uint8_t*)mbt->mods_addr, len);
        }
    }
    if (mboot.flags & MULTIBOOT_INFO_ELF_SHDR) {
        len = mboot.u.elf_sec.num * mboot.u.elf_sec.size;
        mboot.u.elf_sec.addr = earlyalloc(len);
        if (mboot.u.elf_sec.addr) {
            memcpy((uint8_t*)mboot.u.elf_sec.addr, (uint8_t*)mbt->u.elf_sec.addr, len);
        }
    }
    if (mboot.flags & MULTIBOOT_INFO_MEM_MAP) {
        mboot.mmap_addr = earlyalloc(mboot.mmap_length + 4);
        if (mboot.mmap_addr) {
            memcpy((uint8_t*)mboot.mmap_addr,
                    (uint8_t*)mbt->mmap_addr - 4, mboot.mmap_length+4);
            mboot.mmap_addr += 4;
            mboot.mmap_addr = mbt->mmap_addr;
        }
    }
    static char *argv[256];
    // Tokenize!
    i = 1;
    argv[0] = strtok((char*)mboot.cmdline, " ");
    while(argv[i] != NULL) {
        argv[i++] = strtok((char*)mboot.cmdline, " ");
    }

    (void)main(i+1, argv);

    return 0;
}

