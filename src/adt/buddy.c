/*
 * (c) 2019 Apollo Developers
 * For terms, see LICSENSE
 * buddy allocator
 * Allocation: log(n) Free: log(n)
 */

#include <sys/hal.h>
#include <assert.h>
#include <math.h>
#include <adt/buddy.h>

#define BUDDY(x)     (x ^ 1)
#define INC_ORDER(x) (x << 1)
#define DEC_ORDER(x) (x >> 1)

size_t buddy_calc_overhead(range_t r)
{
    size_t accum = 0;
    unsigned i;
    for (i = MIN_BUDDY_SZ_LOG2; i <= MAX_BUDDY_SZ_LOG2; i++) {
        accum += (r.extent >> i) / 8 + 1;
    }
    return accum;
}

int buddy_init(buddy_t *bd, uint8_t *overhead_store, range_t r, int start_freed)
{
    unsigned i, nbits;
    bd->start = r.start;
    bd->size  = r.extent;

    for(i = 0; i < NUM_BUDDY_BUCKETS; i++) {
        nbits = bd->size >> (MIN_BUDDY_SZ_LOG2 + i);
        bitmap_init(&bd->orders[i], overhead_store, nbits);
        overhead_store += nbits / 8 + 1;
    }

    if (start_freed != 0) {
        buddy_free_range(bd, r);
    }
    return 0;
}

uint64_t buddy_alloc(buddy_t *bd, unsigned sz)
{
    unsigned log_sz = log2_roundup(sz);
    if(log_sz > MAX_BUDDY_SZ_LOG2) {
        panic("buddy_alloc had request that was too large to handle");
    }
    unsigned orig_log_sz = log_sz;
    // Search for a free block -- we may need to increase the size of the block
    // to find a free one
    int64_t idx;
    while (log_sz <= MAX_BUDDY_SZ_LOG2) {
        idx = bitmap_first_set(&bd->orders[log_sz - MIN_BUDDY_SZ_LOG2]);
        if(idx != -1) {
            // we found a block
            break;
        }
        log_sz++;
    }

    if (idx == -1) {
        // No free blocks
        return ~0ULL;
    }
    // Split blocks to get a block of minimum size
    for(; log_sz != orig_log_sz; log_sz--) {
        int order_idx = log_sz - MIN_BUDDY_SZ_LOG2;
        // splitting a block, so deallocate it first.
        bitmap_clear(&bd->orders[order_idx], idx);

        // Set both children as free.
        idx = INC_ORDER(idx);
        bitmap_set(&bd->orders[order_idx - 1], idx);
        bitmap_set(&bd->orders[order_idx - 1], idx + 1);
    }
    int order_idx = log_sz - MIN_BUDDY_SZ_LOG2;
    bitmap_clear(&bd->orders[order_idx], idx);

    uint64_t addr = bd->start + ((uint64_t)idx << log_sz);
    return addr;
}

static int aligned_for(uint64_t addr, uintptr_t lg2)
{
    uintptr_t mask = ~( ~0ULL << lg2 );
    return (addr & mask) == 0;
}

void buddy_free(buddy_t *bd, uint64_t addr, unsigned sz)
{
    uint64_t offs   = addr - bd->start;
    unsigned log_sz = log2_roundup(sz);
    unsigned idx    = offs >> log_sz;

    while (log_sz >= MIN_BUDDY_SZ_LOG2) {
        int order_idx = log_sz - MIN_BUDDY_SZ_LOG2;
        // Mark node free
        bitmap_set(&bd->orders[order_idx], idx);
        // Can we coalese up another level?
        if (log_sz == MAX_BUDDY_SZ_LOG2) {
            break;
        }
        if (bitmap_isset(&bd->orders[order_idx], BUDDY(idx)) == 0) {
            // guess not...
            break;
        }
        // Mark both non-free
        bitmap_clear(&bd->orders[order_idx], idx);
        bitmap_clear(&bd->orders[order_idx], BUDDY(idx));

        // Move up an order
        idx = DEC_ORDER(idx);
        log_sz++;
    }
}

void buddy_free_range(buddy_t *bd, range_t range)
{
    unsigned i;
    uint64_t old_start, start;
    uintptr_t sz, min_sz;
    min_sz = 1 << MIN_BUDDY_SZ_LOG2;
    if (aligned_for(range.start, MIN_BUDDY_SZ_LOG2) == 0) {
        if (range.extent < min_sz) {
            return;
        }
        old_start     = range.start;
        range.start  &= ~0ULL << MIN_BUDDY_SZ_LOG2;
        range.start  += min_sz;
        range.extent -= range.start - old_start;
    }
    while (range.extent >= min_sz &&
            aligned_for(range.start, MIN_BUDDY_SZ_LOG2)) {
        for(i = MAX_BUDDY_SZ_LOG2; i >= MIN_BUDDY_SZ_LOG2; i--) {
            sz = 1 << i;
            start = range.start - bd->start;
            if (sz > range.extent || aligned_for(start, i) == 0) {
                continue;
            }
            range.extent -= sz;
            range.start  += sz;
            buddy_free(bd, start + bd->start, sz);
            break;
        }
    }
}

