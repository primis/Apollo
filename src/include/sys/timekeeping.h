/*
 * (c) 2019 Apollo Developers
 * timekeeping.h - Functionality for time
 */

typedef struct timekeeping_state
{
    uint32_t frequency;             // Frequency of clock

    int32_t ticks;                  // Simple number of ticks
    uint32_t rollover;              // Rollover for ticks

    uint32_t tick_per_us;           // Set to 0 if tick > 1 microsecond
    uint32_t us_per_tick;           // Set to 0 if tick < 1 microsecond
    uint32_t us_correction_factor;  // How much to correct
    uint32_t us_correction_rate;    // How often to apply correction
    uint32_t us_correction_count;   // Count for correction

    uint32_t correction_rate;       // How often to apply correction
    uint32_t correction_factor;     // How much to correct
    uint32_t correction_count;      // Count for correction
} timekeeping_state_t;

