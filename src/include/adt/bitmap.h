#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>

// This ADT exposes a statically sized bitmap.

typedef struct bitmap {
    uint8_t *data;
    int64_t max_extent;
} bitmap_t;

void bitmap_init(bitmap_t *xb, uint8_t *storage, int64_t max_extent);

// Set a bit at index idx
void bitmap_set(bitmap_t *xb, unsigned idx);

// Clear a bit at index idx
void bitmap_clear(bitmap_t *xb, unsigned idx);

// Returns nonzero if bit set at idx
int bitmap_isset(bitmap_t *xb, unsigned idx);

// Returns nonzero if bit is clear at idx
int bitmap_isclear(bitmap_t *xb, unsigned idx);

// Return the index of the first bit set, or -1 if no bits are sets
int64_t bitmap_first_set(bitmap_t *xb);

#endif
