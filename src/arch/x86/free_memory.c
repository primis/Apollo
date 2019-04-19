/*
 * (c) 2019 Apollo Developers
 * For terms, See LICENSE
 * free_memory.c - Multiboot header reading to find ranges of free memory
 */

#include <sys/hal.h>
#include <sys/multiboot.h>
#include <stdio.h>
extern multiboot_info_t mboot;

#define MBOOT_IS_MMAP_TYPE_RAM(x) (x == 1)
#define MBOOT_MEM      (1<<0)
#define MBOOT_BOOT_DEV (1<<1)
#define MBOOT_CMDLINE  (1<<2)
#define MBOOT_MODULES  (1<<3)
#define MBOOT_ELF_SYMS (1<<5)
#define MBOOT_MMAP (1<<6)

static void remove_range(range_t *r, uint64_t start, uint64_t extent)
{
    // TODO: Stop assuming that a range exists which starts at 'start' and
    // extends to extent

    // Sanity check to make sure this is the right range
    if(r->start == start) {
        r->start += extent;     // Move range up past extent
        r->extent -= extent;    // Nullify extent
    }
}

static int free_memory()
{
    extern int __start, __end;  // Provided by linker
    if ((mboot.flags & MBOOT_MMAP) == 0) {
        panic("Bootloader did not provide memory map info!");
    }

    // Store ranges in easy format, instead of multiboot
    range_t ranges[128], ranges_cpy[128];

    // Assign I to beginning of the mmap for looping
    uint32_t i      = mboot.mmap_addr;
    unsigned n      = 0;
    uint64_t extent = 0;

    // Iterate until we're at the end of the mmap
    while (i < mboot.mmap_addr + mboot.mmap_length) {
        if (n >= 128) { // Allow 128 loops before a break, avoid looping errors
            break;
        }

        // An entry exists at memory location i, map it to a mmap struct.
        multiboot_memory_map_t *entry = (multiboot_memory_map_t*)i;

        // Is this ram? (Only ram is useful for us)
        if (MBOOT_IS_MMAP_TYPE_RAM(entry->type)) {
            // Make note of start and length of the range, then increment n.
            ranges[n].start = entry->addr;
            ranges[n++].extent = entry->len;

            // Does this new info move out how far RAM goes? Update the extent
            if (entry->addr + entry->len > extent) {
                extent = entry->addr + entry->len;
            }
        }
        uint32_t megs = ((uint32_t)entry->len / 0x100000);
        uint32_t kilos = ((uint32_t)entry->len / 0x400);
        if(megs) {
            printf("Found %d MB at Address 0x%x\n", megs, (uint32_t)entry->addr);
        } else {
            printf("Found %d KB at Address 0x%x\n", kilos, (uint32_t)entry->addr);
        }
        // Go to the next entry in mboot's mmap
        i += entry->size + 4;
    }

    // __end is size of our kernel from ld, we add some flags to it.
    uintptr_t end = (((uintptr_t)&__end) & ~get_page_mask()) + get_page_size();

    // Run over the ranges, one of them has our kernel in it and shouldn't be
    // Marked as free (as our kernel would be overridden)
    for (i = 0; i < n; i++)
        remove_range(&ranges[i], (uintptr_t)&__start, end);

    // Copy the ranges to a backup, as init_physical_memory mutates them and
    // init_cow_refcnts needs to run after init_physical_memory

    for (i = 0; i < n; i++) {
        ranges_cpy[i] = ranges[i];
    }
    init_physical_memory_early(ranges, n, extent);
    init_virtual_memory(ranges, n);
    init_physical_memory();
    init_cow_refcnts(ranges, n);
    return 0;
}

MODULE = {
  .name = "x86/free_memory",
  .required = NULL,
  .load_after = NULL,
  .init = &free_memory,
  .fini = NULL
};
