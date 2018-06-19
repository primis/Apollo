/*
 * (c) 2018 Apollo Project Developers
 * pic.c - Initialize the programmable interrupt controllers
 *
 */

#include <arch/x86/ports.h>
#include <arch/x86/pic.h>

void PIC_init(unsigned char icw1, unsigned char icw3, unsigned char icw4,
        int offset, unsigned short cmd)
{
    unsigned char mask;
    unsigned short data;

    data = cmd + 1;
    mask = inb(data);

    // Start init sequence
    outb(cmd, icw1);// Initialization Control Word 1
    iowait();           // Let the bus catch up
    outb(data, offset); // Where we want the vector to start from
    iowait();
    outb(data, icw3);   // Any slave PICs? (master) or Slave identity (slave)
    iowait();
    outb(data, icw4);   // Mode of the PIC to be set to
    iowait();
    outb(data, mask);   // Restore saved mask
}

void PIC_set_mask(unsigned char line, unsigned short cmd)
{
    unsigned char mask;
    mask = inb(cmd+1) | (1 << line);
    outb(cmd+1, mask);
}

void PIC_clear_mask(unsigned char line, unsigned short cmd)
{
    unsigned char mask;
    mask = inb(cmd+1) & ~(1 << line);
    outb(cmd+1, mask);
}

void PIC_sendEOI(unsigned short command)
{
    outb(command, PIC_EOI);
}

unsigned char PIC_get_IRR(unsigned short command)
{
    outb(command, PIC_READ_IRR);
    return(inb(command));
}

unsigned char PIC_get_ISR(unsigned short command)
{
    outb(command, PIC_READ_ISR);
    return(inb(command));
}
