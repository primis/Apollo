/*
 * (c) 2019 Apollo Developers
 * For terms, see LICENSE
 * early_pmm.c - trivial physical memory manager for pre-virtual memory manager
 * enviorment
 */

#include <sys/hal.h>
#include <assert.h>
#include <string.h>

// Debugging needs printf. defined somewhere else, probably
#ifdef DEBUG_early_pmm
# include <stdio.h>
# define dbg(args...) printf("early_pmm: " args)
#else
# define dbg(args...)
#endif

// The system (another module) gives us ranges of free memory,
// We're gonna modify them in place

range_t early_ranges[128];
unsigned early_nranges;
// Largest valid memory address
uint64_t early_max_extent;

// Copy the ranges. Be simple with it, if something complex happens? Let's panic
int init_physical_memory_early(range_t *ranges, unsigned nranges, uint64_t max)
{
    assert(pmm_init_stage == PMM_INIT_START && "Early mem init called twice!");
    assert(nranges < 128 && "Too many ranges!");
    // You ever read a word too many times and it stops being a word?
    // Well, "ranges"
    // BTW, we're just gonna copy everything here
    memcpy(early_ranges, ranges, nranges * sizeof(range_t));
    early_nranges = nranges;
    early_max_extent = max;

    pmm_init_stage = PMM_INIT_EARLY;
    return 0;
}

uint64_t early_alloc_page()
{
    // This should only be called if the function above was run, but not after
    // the virtual memory is online either
    unsigned i;
    assert(pmm_init_stage == PMM_INIT_EARLY);
    for(i = 0; i < early_nranges; i++) {
        if(early_ranges[i].extent <= 0x1000 ||
           early_ranges[i].start >= 0x100000000ULL) {
            // Throw away pages that exist over 4GB for now, or tinier than 4k.
            continue;
        }
        // Under 1MB? Ignore it!
        if(early_ranges[i].start < 0x100000) {
            continue;
        }
        uint32_t ret = (uint32_t)early_ranges[i].start;
        early_ranges[i].start   += 0x1000;
        early_ranges[i].extent  -= 0x1000;

        dbg("early_allog_page() -> %x\n", ret);
        return ret;
    }
    panic("Early_alloc_page couldn't find any pages to use!");
}



