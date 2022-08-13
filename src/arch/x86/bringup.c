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
static uint32_t earlyalloc(unsigned len)
{
    static uint8_t buf[EARLYALLOC_SZ];
    static unsigned idx = 0;
    if (idx + len >= EARLYALLOC_SZ) {
        // We can't allocate, we ran outta room
        return NULL;
    }
    uint8_t *ptr = &buf[idx];
    idx += len;

    return (uint32_t)ptr;
}

static int tokenize(char tok, char *in, char **out, int maxout) {
  int n = 0;
  
  while(*in && n < maxout) {
    out[n++] = in;

    /* Spool until the next instance of 'tok', or end of string. */
    while (*in && *in != tok)
      ++in;
    /* If we exited because we saw a token, make it a NUL character
       and step over it.*/
    if (*in == tok)
      *in++ = '\0';
  }

  return n;
}

// Called by assembly.
int arch_init(multiboot_info_t* mbt, unsigned int magic)
{
    int argc, len;
    uint32_t i;
    static char *argv[256];
    multiboot_module_t *mod_old, *mod_new;
    
    // Be nice to c++
    extern size_t __ctors_begin;
    extern size_t __ctors_end;
    for (size_t *i = &__ctors_begin; i< &__ctors_end; i++) {
        ((void (*)(void)) *i)();
    }

    memcpy((uint8_t*)&mboot, (uint8_t*)(mbt), sizeof(multiboot_info_t));
    // Now copy info inside to malloc()'d space

    // Command line first
    if (mboot.flags & MULTIBOOT_INFO_CMDLINE) {
        mbt->cmdline += 0xC0000000;
        len = strlen((char*)mbt->cmdline) + 1;
        mboot.cmdline = earlyalloc(len);
        if (mboot.cmdline) {
            memcpy((uint8_t*)mboot.cmdline, (uint8_t*)mbt->cmdline, len);
        }
    }

    if (mboot.flags & MULTIBOOT_INFO_MODS) {
        mbt->mods_addr += 0xC0000000;
        len = mboot.mods_count * sizeof(multiboot_module_t);
        mboot.mods_addr = earlyalloc(len);
        if(mboot.mods_addr) {
            memcpy((uint8_t*)mboot.mods_addr, (uint8_t*)mbt->mods_addr, len);
            // CLI string inside modules
            mod_old = (multiboot_module_t *) mbt->mods_addr;
            mod_new = (multiboot_module_t *) mboot.mods_addr;
            for (i = 0; i < mboot.mods_count; i++)
            {
                len = strlen((char*)mod_old[i].cmdline) + 1;
                mod_new[i].cmdline = earlyalloc(len);
                memcpy((uint8_t*)mod_new[i].cmdline, 
                    (uint8_t*)mod_old[i].cmdline, len);
                mod_new[i].mod_start += 0xC0000000;
                mod_new[i].mod_end   += 0xC0000000;
            }
        }
    }
    if (mboot.flags & MULTIBOOT_INFO_ELF_SHDR) {
        mbt->u.elf_sec.addr += 0xC0000000;
        len = mboot.u.elf_sec.num * mboot.u.elf_sec.size;
        mboot.u.elf_sec.addr = earlyalloc(len);
        if (mboot.u.elf_sec.addr) {
            memcpy((uint8_t*)mboot.u.elf_sec.addr, 
                (uint8_t*)mbt->u.elf_sec.addr, len);
        }
    }
    if (mboot.flags & MULTIBOOT_INFO_MEM_MAP) {
        mbt->mmap_addr += 0xC0000000;
        mboot.mmap_addr = earlyalloc(mboot.mmap_length + 4);
        if (mboot.mmap_addr) {
            memcpy((uint8_t*)mboot.mmap_addr,
                    (uint8_t*)mbt->mmap_addr - 4, mboot.mmap_length+4);
            mboot.mmap_addr += 4;
            mboot.mmap_addr = mbt->mmap_addr;
        }
    }
    
    // Tokenize!
    argc = tokenize(' ', (char*)mboot.cmdline, argv, 256);

    (void)main(argc, argv);

    return 0;
}

