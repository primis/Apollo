/*
 * (C) 2014 Apollo Developers
 * time.h - This is where we keep track of wall time, and internal time.
 */

#include <sys/interrupt.h>
#include <sys/timer.h>

unsigned int epochTime;
unsigned volatile int clockTicks;    // Primary Timer Ticks, Used for wait.

void setAlarm(unsigned int _miliseconds, void_callback_t _function)
{
// Need to implement Malloc for this.
}

void wait(unsigned int _miliseconds)
{
    int temp;
    temp = (_miliseconds / 1000);   // Milisec => Seconds
    temp *= CLOCKS_PER_SEC;         // Seconds => Ticks 
    temp += clockTicks;             // Add in current time
    while(clockTicks < temp){
        ;
    }
}

unsigned int getWallTime()
{
    return epochTime;
}

