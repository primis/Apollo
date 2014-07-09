/*
 * (c) 2014 Apollo Developers
 * pit.c - The Programmable Interrupt Timer, 8253 Compatible Driver 
 */

#include <sys/x86/pit.h>
#include <sys/interrupt.h>
#include <sys/screen.h>


// These are located in core/timer.c
extern unsigned volatile int clockTicks;

void pitCallback(unsigned int _a, ...)
{
    clockTicks++;
    if(clockTicks % CLOCKS_PER_SEC == 0)
    {
        scrWrite("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
        scrWrite("Uptime: ");
        scrWriteHex(clockTicks/CLOCKS_PER_SEC);
    } 
}

void pitInit(unsigned int _frequency)
{
    asm("cli");
    clockTicks = 0;
    unsigned char low, high;
    // PIC Hardware interrupt is IRQ0, Which we relocated to 0x20
    interruptHandlerRegister(0x20, &pitCallback);
    unsigned int divisor = PIT_BASE_FREQ / _frequency;
    outb(PIT_CMD, PIT_CHANNEL_0 | PIT_LHBYTE | PIT_MODE_2 | PIT_BIN_MODE);
    low = (unsigned char)(divisor & 0xFF);
    high= (unsigned char)((divisor >> 8) & 0xFF);
    outb(PIT_DATA_0, low);
    outb(PIT_DATA_0, high);

    clearIRQMask(0);    // Enable the line, Start Interrupting the CPU.

    asm("sti");
}



