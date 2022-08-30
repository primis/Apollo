/*
 * (c) 2022 Apollo Developers
 * For terms, see LICENSE
 * timekeeping.c - Timekeeping functionality setup.
 */

#include <sys/resource.h>
#include <sys/timekeeping.h>
#include <sys/hal.h>
#include <sys/device.h>

extern int i8254_configure(timekeeping_state_t *chip, int channel, int mode);

static resource_t i8254_chip = {
    .name   = "at_pit_resource",
    .start  = 0x40,
    .end    = 0x43,
    .flags  = RESOURCE_IO | RESOURCE_IO_SLOW | RESOURCE_WIDTH_8
};

/* 
 * With the below settings, we get:
 * ~1 KHz timer, with a period of 1.000685561ms
 * After driver corrections, this gives us an accuracy of 18 seconds 
 * loss per year.
 */ 
static timekeeping_state_t state = {
    .name = "at_pit",
    .data = &i8254_chip,
    .frequency = 999,
    .us_per_tick = 1000,
    .ns_per_tick = 685,
    .priority = 100,
    .ns_count = 0,
    .base_frequency = 1193182 /* 1.193181666666666 MHz */
};

/*
 * Channel 0 is connected to the IRQ line, so configure that specifically
 * We want mode 2 (Rate Generator) to pulse the IRQ
 */
static int config[] = {0, 2};

static device_t at_pit = {
    .name   = "at_pit",
    .data   = &state
};

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

static int timekeeping_init()
{
    int interrupt_state = get_interrupt_state();

    // Re-init the timestamp.
    set_timestamp(0);

    // Atomic Section
    disable_interrupts();

    if(device_register(&at_pit, base_device(), "clock/i8254")) {
        return -1;
    }
    if(device_init(&at_pit, config)) {
        return -1;
    }

    // IRQ 0 is mapped to interrupt 32.
    register_interrupt_handler(32, &timekeeping, NULL);

    // End Atomic Section
    set_interrupt_state(interrupt_state);
    return 0;
}

static prereq_t prereqs[] = {{"interrupts", NULL}, {NULL, NULL}};

MODULE = {
    .name = "timekeeping",
    .required = prereqs,
    .load_after = NULL,
    .init = &timekeeping_init,
    .fini = NULL,
};
