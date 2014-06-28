/*
 * (c) 2014 Apollo Developers
 * <sys/timer.h> Functions related to timing and alarms
 */
#ifndef _SYS_TIMER_H
#define _SYS_TIMER_H

#include <sys/callback.h>
#include <time.h>

struct alarm_struct {
    void_callback_t callback;
    unsigned int ticks;
    struct alarm_struct *next;
}; 
typedef struct alarm_struct alarm_t;

void setAlarm(unsigned int miliseconds, void_callback_t function);
void wait(unsigned int miliseconds);
unsigned int getWallTime();
#endif

