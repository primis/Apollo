/*
 * (c) 2019 Apollo Developers
 * For terms, see LICENSE
 * bitmap.c - ADT for bitmap storage
 */

#include <adt/bitmap.h>
#include <sys/hal.h>
#include <assert.h>
#include <string.h>

// Return index of lsb that is set in byte. byte==0 undefined.

static int lsb_set(uint8_t byte)
{
    int i = 0;
    while ((byte & 1) == 0) {
        i++;
        byte >>= 1;
    }
    return i;
}

void bitmap_init(bitmap_t *xb, uint8_t *storage, int64_t max_extent)
{
    xb->max_extent  = max_extent;
    xb->data        = storage;
    memset(xb->data, 0, max_extent / 8 + 1);
}

void bitmap_set(bitmap_t *xb, unsigned idx)
{
    xb->data[idx/8] |= (1 << (idx % 8));
    assert(bitmap_isset(xb, idx));
}

void bitmap_clear(bitmap_t *xb, unsigned idx)
{
    xb->data[idx/8] &= ~(1 << (idx % 8));
}

int bitmap_isset(bitmap_t *xb, unsigned idx)
{
    return (xb->data[idx/8] & (1 << (idx % 8))) ? 1 : 0;
}

int bitmap_isclear(bitmap_t *xb, unsigned idx)
{
    return !bitmap_isset(xb, idx);
}

int64_t bitmap_first_set(bitmap_t *xb)
{
    uint64_t i;
    int64_t  idx;
    for (i = 0; i < (xb->max_extent >> 3) + 1ULL; i++) {
        if (xb->data[i] == 0) {
            continue;
        }
        idx = i * 8 + lsb_set(xb->data[i]);
        return (idx > xb->max_extent) ? -1 : idx;
    }
    return -1;
}
