/*
 * rtc-cmos.h - RTC and CMOS definitions
 */

#ifndef __RTC_CMOS_H
#define __RTC_CMOS_H

#include <stdint.h>

typedef struct cmos_register {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
    uint8_t century;

    uint8_t registera;
    uint8_t registerb;
    uint8_t registerc;
    uint8_t registerd;
} cmos_register_t;

#define CMOS_INDEX_PORT         0x70    // TODO: Move these to a config
#define CMOS_DATA_PORT          0x71

#define CMOS_REGISTER_SECOND    0x00
#define CMOS_REGISTER_MINUTE    0x02
#define CMOS_REGISTER_HOUR      0x04
#define CMOS_REGISTER_DAY       0x07
#define CMOS_REGISTER_MONTH     0x08
#define CMOS_REGISTER_YEAR      0x09
#define CMOS_REGISTER_STATUSA   0x0A
#define CMOS_REGISTER_STATUSB   0x0B
#define CMOS_REGISTER_STATUSC   0x0C
#define CMOS_REGISTER_STATUSD   0x0D

#define CMOS_BASE_FREQUENCY     32768

// Register A flags
#define CMOS_UPDATE_IN_PROGRESS 0x80
// Bits 4-6 are frequency divider for base clock, you shouldn't touch that
// Bits 0-3 are the interrupt frequency divider

// Register B flags
#define CMOS_UPDATE_CLOCK       0x80
#define CMOS_PERIODIC_INTERRUPT 0x40
#define CMOS_ALARM_INTERRUPT    0x20
#define CMOS_UPDATE_ENDED_INT   0x10
#define CMOS_SQUARE_WAVE        0x08
#define CMOS_BINARY_DATE_MODE   0x04
#define CMOS_24_HOUR_MODE       0x02
#define CMOS_DAYLIGHT_SAVINGS   0x01

// Register C flag
#define CMOS_INTERRUPT_REQUEST  0x80
#define CMOS_INTERRUPT_OCCURED  0x40
#define CMOS_ALARM_OCCURED      0x20
#define CMOS_UPDATE_ENDED_OCCURED 0x10
// Bottom 4 bits are reserved


// Register D flags
#define CMOS_VALID_RAM          0x80
// The bottom 7 bits are reserved.

#endif
