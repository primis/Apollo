/*
 * (c) 2019 Apollo Developers
 * For terms, See LICENSE
 * free_memory.c - Multiboot header reading to find ranges of free memory
 */

#include <sys/hal.h>
#include <sys/multiboot.h>
#include <stdio.h>
extern multiboot_info_t mboot;

#define MBOOT_IS_MMAP_TYPE_RAM(x) (x == MULTIBOOT_MEMORY_AVAILABLE)
#define MBOOT_MEM      (1<<0)
#define MBOOT_MMAP (1<<6)

static uint32_t total_megabytes;

static void remove_range(range_t *r, uint32_t start, uint32_t end)
{
    // TODO: Stop assuming that a range exists which starts at 'start' and
    // extends to extent
    // Sanity check to make sure this is the right range

    if(r->start == start) {
        r->start += end;     // Move range up past extent
        r->extent -= end;    // lower extent
    }
}

static int free_memory()
{
    extern uint32_t __start, __end;  // Provided by linker

    // Store ranges in easy format, instead of multiboot
    range_t ranges[128], ranges_cpy[128];
    uint32_t i = 0;
    unsigned n      = 0;
    uint64_t extent = 0;
    multiboot_memory_map_t *entry;

    if ((mboot.flags & MBOOT_MMAP) == 0) {
        panic("Bootloader did not provide memory map info!");
    }

    // Iterate until we're at the end of the mmap
    for (entry = (multiboot_memory_map_t *) mboot.mmap_addr;
           (uint32_t)entry < mboot.mmap_addr + mboot.mmap_length;
           entry = (multiboot_memory_map_t *) ((uint32_t) entry + entry->size +
            sizeof (entry->size))) {
        if (n >= 128) { // Allow 128 loops before a break, avoid looping errors
            break;
        }

        // Check for a non-sensical MMAP Type, assume that they're RAM
        if(entry->type > 5)
        {
            entry->type = MULTIBOOT_MEMORY_AVAILABLE;
        }

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
        i++;
    }
    
    // __end is size of our kernel from ld, we add some flags to it.
    uint32_t end = (((uint32_t)&__end) & ~get_page_mask()) + get_page_size();
    // Run over the ranges, one of them has our kernel in it and shouldn't be
    // Marked as free (as our kernel would be overridden)
    for (i = 0; i < n; i++)
    {
        remove_range(&ranges[i], (uint32_t)&__start, end);
    }

    // Copy the ranges to a backup, as init_physical_memory mutates them and
    // init_cow_refcnts needs to run after init_physical_memory

    for (i = 0; i < n; i++) {
        ranges_cpy[i] = ranges[i];
    }
    init_physical_memory_early(ranges, n, extent);
    init_virtual_memory(ranges, n);
    init_physical_memory();
    init_cow_refcnts(ranges, n);
    total_megabytes = (mboot.mem_upper / 1024) + 2;
    return 0;
}

uint32_t free_memory_get_megs()
{
    return total_megabytes;
}

MODULE = {
  .name = "x86/free_memory",
  .required = NULL,
  .load_after = NULL,
  .init = &free_memory,
  .fini = NULL
};
