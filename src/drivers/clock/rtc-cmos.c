/*
 * (c) 2022 Apollo Developers
 * For terms, see LICENSE
 * rtc.c - Real time clock and the CMOS backup battery
 */

#include <sys/hal.h>
#include <sys/resource.h>
#include "include/rtc_cmos.h"

// This is cumulative days, not real "days"
static const uint32_t days_from_month[12] = {
// Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
     0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334
};
static const uint32_t seconds_in_a_day  = 86400;
static const uint32_t seconds_in_a_year = 31536000;

static resource_t rtc = {
    .name   = "cmos-registers",
    .start  = 0x70,
    .end    = 0x71,
    .flags  = RESOURCE_IO | RESOURCE_IO_INDEXED | RESOURCE_IO_SLOW
};

// Returns number of leap days since beginning of epoch (January 1970)
// This doesn't work for divisible by 100 non-leap years (2100, for example)
// TODO: Fix that ^
static int leap_days(uint8_t year, uint8_t month)
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

static int cmos_update_in_progress()
{
    uint8_t ret;
    resource_read(&ret, &rtc, CMOS_REGISTER_STATUSA, 1);
    return ret & 0x80;
}

static void read_RTC(cmos_register_t *cmos)
{
    int interrupt_state;
    
    while(cmos_update_in_progress()); // Wait until CMOS finished an update

    // Atomic section
    interrupt_state = get_interrupt_state();
    disable_interrupts();
    resource_read(&(cmos->second   ), &rtc, CMOS_REGISTER_SECOND, 1);
    resource_read(&(cmos->minute   ), &rtc, CMOS_REGISTER_MINUTE, 1);
    resource_read(&(cmos->hour     ), &rtc, CMOS_REGISTER_HOUR,   1);
    resource_read(&(cmos->day      ), &rtc, CMOS_REGISTER_DAY,    1);
    resource_read(&(cmos->month    ), &rtc, CMOS_REGISTER_MONTH,  1);
    resource_read(&(cmos->year     ), &rtc, CMOS_REGISTER_YEAR,   1);
    resource_read(&(cmos->registera), &rtc, CMOS_REGISTER_STATUSA,1);
    resource_read(&(cmos->registerb), &rtc, CMOS_REGISTER_STATUSB,1);
    resource_read(&(cmos->registerc), &rtc, CMOS_REGISTER_STATUSC,1);
    resource_read(&(cmos->registerd), &rtc, CMOS_REGISTER_STATUSD,1);
    // End of Atomic section
    set_interrupt_state(interrupt_state);


    // Check if the time is in BCD format
    if (!(cmos->registerb & CMOS_BINARY_DATE_MODE)) {
        // Convert out of BCD, very simple
        cmos->second = (cmos->second & 0xF) + ((cmos->second / 16) * 10);
        cmos->minute = (cmos->minute & 0xF) + ((cmos->minute / 16) * 10);
        cmos->hour   = (cmos->hour   & 0xF) + ((cmos->hour   / 16) * 10);
        cmos->day    = (cmos->day    & 0xF) + ((cmos->day    / 16) * 10);
        cmos->month  = (cmos->month  & 0xF) + ((cmos->month  / 16) * 10);
        cmos->year   = (cmos->year   & 0xF) + ((cmos->year   / 16) * 10);
    }
    // Assume it's not 21xx year
    cmos->year += 2000;
}

// Returns the number of seconds since January 1st, 1970.
uint32_t get_epoch_time()
{
    uint32_t time;
    cmos_register_t cmos;

    // Populate cmos
    read_RTC(&cmos);
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
