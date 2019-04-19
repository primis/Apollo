/*
 * (c) 2019 Apollo Developers
 * For terms, see LICENSE
 * vmm.c - x86 virtual memory manager - requires a 486 or higher
 */

#include <sys/hal.h>
#include <stdio.h>
#include <string.h>
#include <arch/x86/regs.h>
#include <arch/x86/ports.h>

#ifdef DEBUG_vmm
# define dbg(args...) printf("vmm: " args)
#else
# define dbg(args...)
#endif

// Defined in platform specific HAL
static address_space_t *current = NULL;

static spinlock_t global_vmm_lock = SPINLOCK_RELEASED;

// Helper functions to map x86-specific page flags

static int from_x86_flags(int flags)
{
    int f = 0;
    if (flags & X86_WRITE)      f |= PAGE_WRITE;
    if (flags & X86_EXECUTE)    f |= PAGE_EXECUTE;
    if (flags & X86_USER)       f |= PAGE_USER;
    if (flags & X86_COW)        f |= PAGE_COW;
    return f;
}

static int to_x86_flags(int flags)
{
    int f = 0;
    if (flags & PAGE_WRITE)   f |= X86_WRITE;
    if (flags & PAGE_USER)    f |= X86_USER;
    if (flags & PAGE_EXECUTE) f |= X86_EXECUTE;
    if (flags & PAGE_COW)     f |= X86_COW;
    return f;
}

address_space_t *get_current_address_space()
{
    return current;
}

int switch_address_space(address_space_t *dest)
{
    write_cr3((uintptr_t)dest->directory | X86_PRESENT | X86_WRITE);
    return 0;
}
