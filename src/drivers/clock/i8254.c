/*
 * (c) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * i8254.c - Driver for the 8253/8254 Programmable Interrupt Timer
 */

#include <arch/x86/ports.h>
#include <sys/hal.h>
#include <sys/timekeeping.h>
#include <stdint.h>

#include "include/i8254.h"

static timekeeping_state_t state; 

// Desired Frequency, us per tick, fractional nanoseconds leftover
// 999Hz desired setting gives 1.000685561 ms per tick 
// That's an accuracy loss (with corrections) of 18 seconds per year
static const uint32_t i8254_config[] = { 36157, 27, 657 };

static int timekeeping(struct regs *regs, void *p)
{
    // Grab current timestamp so we can update it
    uint64_t ts = get_timestamp();

    state.ns_count += state.ns_per_tick;
    if(state.ns_count >= 1000) { // Did we hit a microsecond?
        ts += (state.ns_count / 1000);
        state.ns_count %= 1000;
    }
    // Add Microseconds to the clock.
    ts += state.us_per_tick;
    set_timestamp(ts);
    return 0;
}

static int i8254_init()
{
    uint32_t divisor;
    uint8_t low, high, init_byte;

    int interrupt_state;

    // Nearest integer frequency of the PIT
    state.frequency     = i8254_config[0];
    // Calculated Microseconds per tick (from real frequency)
    state.us_per_tick   = i8254_config[1];
    // Calculated Nanoseconds per tick (3 Significant Figures, only the ns part)
    state.ns_per_tick   = i8254_config[2];
    state.ns_count      = 0;
    state.priority      = 100;

    // Re-init the timestamp.
    set_timestamp(0);

    init_byte = i8254_CHANNEL_0 | i8254_LHBYTE | i8254_MODE_2 | i8254_BIN_MODE;
    divisor   = i8254_BASE_FREQ / i8254_config[0];
    low       = (uint8_t)(divisor & 0xFF);
    high      = (uint8_t)((divisor >> 8) & 0xFF);

    interrupt_state = get_interrupt_state();
    disable_interrupts();

    // Atomic Section

    write_register(i8254_DATA_PORT, i8254_CMD, init_byte);
    write_register(i8254_DATA_PORT, i8254_DATA_0, low);
    write_register(i8254_DATA_PORT, i8254_DATA_0, high);
    // PIC is on IRQ 0, which is int 32.
    register_interrupt_handler(32, &timekeeping, NULL);

    set_interrupt_state(interrupt_state);
    // End Atomic Section

    return 0;
}

static prereq_t prereqs[] = {{"interrupts", NULL}, {NULL, NULL}};

MODULE = {
    .name = "timekeeping",
    .required = prereqs,
    .load_after = NULL,
    .init = &i8254_init,
    .fini = NULL,
};
