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
// 36.157 KHz, 99 ticks per correction, 2 tick correction.
// 27 Microseconds per tick, 3 ticks per correction, 2 uS correction.
static const uint32_t PIT_config[] = { 36157, 99, 2, 27, 3, 2 };

static int timekeeping(struct regs *regs, void *p)
{
    // Grab current timestamp so we can update it
    uint64_t ts = get_timestamp();

    timekeeping_state_t *state = (timekeeping_state_t*)p;
    state->ticks            += 1;
    state->correction_count += 1;
    state->us_correction_count++;

    if(state->us_correction_count >= state->us_correction_rate) {
        ts += state->us_correction_factor;
        state->us_correction_count = 0;
    }

    if (state->correction_count >= state->correction_rate) {
        // Add ticks that we missed from the rounding error
        state->ticks += state->correction_factor;
        // Factor into the system clock
        ts += (state->us_per_tick * state->correction_factor);
        state->correction_count = 0;
    }

    // The PIT is set to around 27 microseconds
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

    state.frequency            = PIT_config[0];
    state.correction_rate      = PIT_config[1];
    state.correction_factor    = PIT_config[2];
    state.us_per_tick          = PIT_config[3];
    state.us_correction_factor = PIT_config[4];
    state.us_correction_rate   = PIT_config[5];
    state.tick_per_us          = 0;
    state.ticks                = 0;
    state.correction_count     = 0;
    state.us_correction_count  = 0;

    set_timestamp(0);

    init_byte = PIT_CHANNEL_0 | PIT_LHBYTE | PIT_MODE_2 | PIT_BIN_MODE;
    divisor   = PIT_BASE_FREQ / PIT_config[0];
    low       = (uint8_t)(divisor & 0xFF);
    high      = (uint8_t)((divisor >> 8) & 0xFF);

    interrupt_state = get_interrupt_state();
    disable_interrupts();

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
