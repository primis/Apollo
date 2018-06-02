/*
 * (c) 2016 Apollo Project Developers
 * Main.c - Main function of kernel activity
 */
#include <sys/console.h>
#include <sys/core.h>
void main()
{
    console_write("Base System Initialized");
}


// Small (K)ernel (St)atic alloc
// Function is used for quickly allocating memory that
// is never planned on being freed early in the kernel task.

static unsigned char* buf[KSTALLOC_SZ];

unsigned int kstalloc(unsigned int len)
{
    static unsigned idx = 0;
    if (idx + len >= KSTALLOC_SZ)
    {
        // We ran outta space, oops!
        // time to return early
        return -1;
    }

    unsigned char* ptr = (unsigned char *) &buf[idx];
    idx += len;
    return (unsigned int)ptr;
}

