/*
 * (c) 2019 Apollo Developers
 * timekeeping.h - Functionality for time
 */

#ifndef __SYS_TIMEKEEPING_H
#define __SYS_TIMEKEEPING_H

#include <stdint.h>
#include <sys/resource.h>

typedef struct callback_state
{
    // Data given by register_callback()
    uint64_t miliseconds;
    int periodic;
    void (*cb)(void *);
    void *data;

    // Internal bookkeeping
    uint64_t time_elapsed;
    int active;
} callback_state_t;

// Struct for a timekeeping device
typedef struct timekeeping_state
{
    const char *name;       // Name of this timekeeping device
    uint32_t frequency;     // Frequency of clock
    uint64_t ticks;         // Simple number of ticks

    uint32_t priority;      // Priority to set system clock (highest wins)

    uint32_t us_per_tick;   // Microseconds per tick (rounded)
    uint32_t ns_per_tick;   // Remainder from microsecond round (3 places)
    uint32_t ns_count;      // Count of ns, so we can add it to timestamp

    uint32_t base_frequency;// Frequency of base clock
    resource_t *data;       // Pointer to resource struct to access the device
} timekeeping_state_t;

#endif
