/*
 * (c) 2018 Apollo Project Developers
 * For terms, see LICENSE
 * ports.c - x86 I/O databus commends
 */

#include <arch/x86/ports.h>

void outb(uint16_t port, uint8_t value)
{
    __asm__ __volatile__ ("outb %0, %1" : : "dN" (port), "a" (value));
}

uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ __volatile__ ("inb %0, %1" : "=a" (ret) : "dN" (port));
    return ret;
}

uint16_t inw(uint16_t port)
{
    uint16_t ret;
    __asm__ __volatile__ ("inw %0, %1" : "=a" (ret) : "dN" (port));
    return ret;
}

void iowait(void)
{
    // Port 0x80 is used for POST checkpoints, so it's safe to use
    // As a "scrap" port to throw data to as a bus wait.
    // On a side note, port 0x80 shows up on certain MoBo's as a diagnostic
    // for POST Codes, so that's kinda cool.
    __asm__ __volatile__ ("outb 0x80, %0" : : "a" ((unsigned char)0));
}


// Write to an offset port given a base
void write_register(int base, int reg, uint8_t value)
{
    outb((uint16_t)base+reg, value);
}

// Same, but read
uint8_t read_register(int base, int reg)
{
    return inb((uint16_t)base+reg);
}

inline uint32_t read_cr0()
{
    uint32_t ret;
    __asm__ __volatile__ ("mov %0, cr0" : "=r" (ret));
    return ret;
}

inline uint32_t read_cr2()
{
    uint32_t ret;
    __asm__ __volatile__ ("mov %0, cr2" : "=r" (ret));
    return ret;
}

inline uint32_t read_cr3()
{
    uint32_t ret;
    __asm__ __volatile__ ("mov %0, cr3" : "=r" (ret));
    return ret;
}

inline void write_cr0(uint32_t val)
{
    __asm__ __volatile__ ("mov cr0, %0" : : "r" (val));
}

inline void write_cr2(uint32_t val)
{
    __asm__ __volatile__ ("mov cr2, %0" : : "r" (val));
}

inline void write_cr3(uint32_t val)
{
    __asm__ __volatile__ ("mov cr3, %0" : : "r" (val));
}
