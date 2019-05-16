/*
 * (c) 2019 Apollo Developers
 * For terms, see LICENSE
 * math.c - math helper functions
 */

#include <math.h>

unsigned log2_roundup(unsigned n)
{
    // Calculate the floor of log2(n)
    unsigned l2;

    l2 = 31 - __builtin_clz(n); // gcc builtin, returns number of leading 0's
                                // in n, if x is 0, result undefined.
                                // TODO: I hate using builtins
    if (n == 1U << l2) {        // n == 2^log2(n), floor(n) = n
        return l2;              // no need to round up
    }
    // floor(n) != n, return round up
    return l2 + 1;

}
