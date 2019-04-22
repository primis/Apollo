/*
 * (c) 2018 Apollo Project Developers
 * For terms, see LICENSE
 * pit.c - Initialize the programmable interrupt timer
 * Driver for the 8253/8254
 */

#include <arch/x86/ports.h>
#include <arch/x86/pit.h>
#include <sys/hal.h>
#include <sys/timekeeping.h>
#include <stdio.h>

// Desired Frequency, us per tick, fractional nanoseconds leftover
static const uint32_t PIT_config[] = { 36157, 27, 657 };

static int timekeeping(struct regs *regs, void *p)
{
    // Grab current timestamp so we can update it
    uint64_t ts = get_timestamp();

    timekeeping_state_t *state = (timekeeping_state_t*)p;
    state->ns_count += state->ns_per_tick;
    if(state->ns_count >= 1000) { // Did we hit a microsecond?
        ts += state->ns_count / 1000;
        state->ns_count %= 1000;
    }
    // Add Microseconds to the clock.
    ts += state->us_per_tick;
    set_timestamp(ts);
    return 0;
}

static int PIT_init()
{
    uint32_t divisor;
    uint8_t low, high, init_byte;

    timekeeping_state_t state;
    int interrupt_state;

    // Nearest integer frequency of the PIT
    state.frequency     = PIT_config[0];
    // Calculated Microseconds per tick (from real frequency)
    state.us_per_tick   = PIT_config[1];
    // Calculated Nanoseconds per tick (3 Significant Figures, only the ns part)
    state.ns_per_tick   = PIT_config[2];
    state.ns_count      = 0;

    // Re-init the timestamp.
    set_timestamp(0);

    init_byte = PIT_CHANNEL_0 | PIT_LHBYTE | PIT_MODE_2 | PIT_BIN_MODE;
    divisor   = PIT_BASE_FREQ / PIT_config[0];
    low       = (uint8_t)(divisor & 0xFF);
    high      = (uint8_t)((divisor >> 8) & 0xFF);

    interrupt_state = get_interrupt_state();
    disable_interrupts();

    // Atomic Section

    write_register(PIT_DATA_PORT, PIT_CMD, init_byte);
    write_register(PIT_DATA_PORT, PIT_DATA_0, low);
    write_register(PIT_DATA_PORT, PIT_DATA_0, high);
    // PIC is on IRQ 0, which is int 32.
    register_interrupt_handler(32, &timekeeping, (void*)&state);

    set_interrupt_state(interrupt_state);

    return 0;
}

static prereq_t prereqs[] = {{"interrupts", NULL}, {NULL, NULL}};

MODULE = {
    .name = "timekeeping",
    .required = prereqs,
    .load_after = NULL,
    .init = &PIT_init,
    .fini = NULL,
};
