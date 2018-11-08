/*
 * (c) 2018 Apollo Project Developers
 * pic.c - Initialize the programmable interrupt controllers
 *
 */

#include <arch/x86/ports.h>
#include <arch/x86/pic.h>
#include <arch/x86/idt.h>

/**
 ** Generic PIC driver code
 **/

void PIC_init(uint8_t icw1, uint8_t icw3, uint8_t icw4, int off, uint16_t cmd)
{
    unsigned char mask;
    unsigned short data;

    data = cmd + 1;
    mask = inb(data);
    icw1 |= PIC_ICW1_INIT;

    if(icw4) {
        icw1 |= PIC_ICW1_ICW4;
    }

    // Start init sequence
    outb(cmd, icw1);// Initialization Control Word 1
    iowait();           // Let the bus catch up
    outb(data, off); // Where we want the vector to start from
    iowait();
    outb(data, icw3);   // Any slave PICs? (master) or Slave identity (slave)
    iowait();
    outb(data, icw4);   // Mode of the PIC to be set to
    iowait();
    outb(data, mask);   // Restore saved mask
}

void PIC_set_mask(uint8_t line, uint16_t cmd)
{
    unsigned char mask;
    mask = inb(cmd+1) | (1 << line);
    outb(cmd+1, mask);
}

void PIC_clear_mask(uint8_t line, uint16_t cmd)
{
    unsigned char mask;
    mask = inb(cmd+1) & ~(1 << line);
    outb(cmd+1, mask);
}

void PIC_sendEOI(uint16_t command)
{
    outb(command, PIC_EOI);
}

uint8_t PIC_get_IRR(uint16_t command)
{
    outb(command, PIC_READ_IRR);
    return(inb(command));
}

uint8_t PIC_get_ISR(uint16_t command)
{
    outb(command, PIC_READ_ISR);
    return(inb(command));
}
