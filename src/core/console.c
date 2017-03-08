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

void console_write_hex(unsigned int n) {
    int i, tmp;
    int noZero = 1; // Use as bool
    console_write("0x");
    for(i=28; i>=0; i-=4)
    {
        tmp = (n >> i) & 0xF;
        if(tmp==0 && noZero!=0)
        {
            continue;
        } else if (tmp >= 0xA) {
            noZero = 0;
            console_put(tmp - 0xA + 'A');
        } else {
            noZero = 0;
            console_put(tmp + '0');
        }
    }
}
