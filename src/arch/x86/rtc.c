/*
 * (c) 2019 Apollo Developers
 * For terms, see LICENSE
 * rtc.c - Real time clock and the CMOS backup battery
 * TODO: periodic interrupts
 */

#include <sys/hal.h>
#include <arch/x86/ports.h>
#include <arch/x86/cmos.h>
// This is cumulative days, not real "days"
const uint32_t days_from_month[12] = {
// Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
     0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334
};
const uint32_t seconds_in_a_day  = 86400;
const uint32_t seconds_in_a_year = 31536000;
static void read_RTC(cmos_register_t cmos);



// Returns number of leap days since beginning of epoch (January 1970)
// This doesn't work for divisible by 100 non-leap years (2100, for example)
// TODO: Fix that ^
int leap_days(uint8_t year, uint8_t month)
{
    int days;
    year -= 1972; // 1972 was a leap year (good starting point)
    days = (year / 4) + 1; // Add one for 1972

    // If it's not march or later in a leap year, fix the off by one error
    if ((year/4 == 0) && (month <= 2)) {
        days--;
    }
    return days;
}

// Returns the number of seconds since January 1st, 1970.
uint32_t get_time()
{
    uint32_t time;
    // Init one of these so gcc doesn't yell at us
    cmos_register_t cmos = {.day = 0};

    // Populate cmos
    read_RTC(cmos);
    // Start adding seconds
    time =  cmos.second;
    time += cmos.minute                 * 60;
    time += cmos.hour                   * 3600;
    time += cmos.day                    * seconds_in_a_day;
    time += days_from_month[cmos.month] * seconds_in_a_day;
    time += (cmos.year - 1970)          * seconds_in_a_year;

    // Add in Leap year's extra days
    time += (leap_days(cmos.year, cmos.month) * seconds_in_a_day);

    return time;
}
static uint8_t get_CMOS_register(uint8_t reg)
{
    // It's important for this to be atomic, since we're grabbing a timestamp
    int interrupt_state = get_interrupt_state();
    uint8_t ret;
    disable_interrupts();
    outb(CMOS_INDEX_PORT, reg);
    ret = inb(CMOS_DATA_PORT);
    set_interrupt_state(interrupt_state);
    return ret;
}

static int cmos_update_in_progress()
{
    uint8_t ret;
    ret = get_CMOS_register(CMOS_REGISTER_STATUSA);
    return ret & 0x80;
}

static void read_RTC(cmos_register_t cmos)
{
    while(cmos_update_in_progress()); // Wait until CMOS finished an update

    cmos.second     = get_CMOS_register(CMOS_REGISTER_SECOND);
    cmos.minute     = get_CMOS_register(CMOS_REGISTER_MINUTE);
    cmos.hour       = get_CMOS_register(CMOS_REGISTER_HOUR);
    cmos.day        = get_CMOS_register(CMOS_REGISTER_DAY);
    cmos.month      = get_CMOS_register(CMOS_REGISTER_MONTH);
    cmos.year       = get_CMOS_register(CMOS_REGISTER_YEAR);
    cmos.registera  = get_CMOS_register(CMOS_REGISTER_STATUSA);
    cmos.registerb  = get_CMOS_register(CMOS_REGISTER_STATUSB);
    cmos.registerc  = get_CMOS_register(CMOS_REGISTER_STATUSC);
    cmos.registerd  = get_CMOS_register(CMOS_REGISTER_STATUSD);
    // Check if the time is in BCD format
    if (!(cmos.registerb & CMOS_BINARY_DATE_MODE)) {
        // Convert out of BCD, very simple
        cmos.second = (cmos.second & 0xF) + ((cmos.second / 16) * 10);
        cmos.minute = (cmos.minute & 0xF) + ((cmos.minute / 16) * 10);
        cmos.hour   = (cmos.hour   & 0xF) + ((cmos.hour   / 16) * 10);
        cmos.day    = (cmos.day    & 0xF) + ((cmos.day    / 16) * 10);
        cmos.month  = (cmos.month  & 0xF) + ((cmos.month  / 16) * 10);
        cmos.year   = (cmos.year   & 0xF) + ((cmos.year   / 16) * 10);
    }
    // Assume it's not 21xx year
    cmos.year += 2000;
}
