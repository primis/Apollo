/*
 * (c) 2016 Apollo Project Developers
 * core/interrupts.c - cpu agnostic interrupt code
 */

#include <sys/interrupts.h>

voidCallback interruptHandlers[256];

void unhandledInterrupt(int argc, reg_t *argv)
{
}

void interruptHandlerRegister(int num, voidCallback handler)
{
    interruptHandlers[num] = handler;
}
