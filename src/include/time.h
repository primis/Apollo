/*
 * <time.h> This header file contains definitions of functions to get
 * and manipulate date and time information. Following the SUS V2
 * Implementation
 */

#ifndef _TIME_H
#define _TIME_H

#define CLOCKS_PER_SEC 1000

typedef unsigned int time_t;
struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};
#endif
