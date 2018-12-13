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

    mask = read_register(cmd, 1);
    icw1 |= PIC_ICW1_INIT;

    if(icw4) {
        icw1 |= PIC_ICW1_ICW4;
    }

    // Start init sequence
    write_register(cmd, 0, icw1);
    iowait();           // Let the bus catch up
    write_register(cmd, 1, off); // Where we want the vector to start from
    iowait();
    write_register(cmd, 1, icw3); // Any slave PICs, or Slave identity
    iowait();
    write_register(cmd, 1, icw4); // Mode of the PIC to be set to
    iowait();
    write_register(cmd, 1, mask); // Restore saved mask
}

void PIC_set_mask(uint8_t line, uint16_t cmd)
{
    unsigned char mask;
    mask = read_register(cmd, 1) | (1 << line);
    write_register(cmd, 1, mask);
}

void PIC_clear_mask(uint8_t line, uint16_t cmd)
{
    unsigned char mask;
    mask = read_register(cmd, 1) & ~(1 << line);
    write_register(cmd, 1, mask);
}

void PIC_sendEOI(uint16_t command)
{
    write_register(command, 0, PIC_EOI);
}

uint8_t PIC_get_IRR(uint16_t command)
{
    write_register(command, 0, PIC_READ_IRR);
    return(read_register(command, 0));
}

uint8_t PIC_get_ISR(uint16_t command)
{
    write_register(command, 0, PIC_READ_ISR);
    return(read_register(command, 0));
}
