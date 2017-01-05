/*
 * (c) 2017 Apollo Project Developers
 * core/console.c - Console interface for early kernel debugging
 */

#include <sys/console.h>

void console_write(char *c) {
    int i=0;
    while (c[i])
    {
        // defined on a per-arch basis.
        console_put(c[i++]);
    }
}
