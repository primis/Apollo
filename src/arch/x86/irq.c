/*
 * (C) 2018 Apollo Project Developers
 * irq.c - Interrupt Request Lines (hardware)
 */

#include <arch/x86/pic.h>
#include <arch/x86/idt.h>
#include <sys/hal.h>

void acknowledge_IRQ(unsigned n)
{
    // Is this from the Slave PIC?
    if (n >= 40 && n < 48) {
        PIC_sendEOI(PIC2_CMD);
    }
    // Is this from either pic?
    if (n >= 32 && n < 48) {
        PIC_sendEOI(PIC1_CMD);
    }
}

void enable_IRQ(uint8_t line, unsigned enable)
{
    uint16_t port;

    // Check which PIC to address if it's a hardware IRQ.
    if (line < 8) {
        port = PIC1_CMD + 1;    // PIC Data is offset 1
    } else if (line < 16) {
        port = PIC2_CMD + 1;
        line -= 8;
    } else {
        return;
    }

    if (enable) {               // Are we enabling or disabling?
        PIC_clear_mask(line, port);
    } else {
        PIC_set_mask(line, port);
    }
}

int init_irq()
{
    int i;

    // Mask all interrupts
    for(i = 0; i < 16; i++) {
        enable_IRQ(i, 0);
    }

    // 0 is ICW1 because we don't need any additional features of it.
    // Remap PIC 1 to offset 32. 0x4 is the location of the second pic: 00001000
    PIC_init(0, 0x4, PIC_ICW4_8086, 32, PIC1_CMD);
    // Remap PIC 2 to offset 40, 0x02 is the idenity line on master slave
    PIC_init(0, 0x2, PIC_ICW4_8086, 40, PIC2_CMD);

    // Unmask the cascade port for PIC 2
    enable_IRQ(2, 1);

    ack_irq = &acknowledge_IRQ;
    enable_irq = &enable_IRQ;
    return 0;
}

static prereq_t prereqs[] = { {"interrupts", NULL}, {NULL, NULL} };

MODULE = {
    .name = "x86/irq",
    .required = prereqs,
    .load_after = NULL,
    .init = &init_irq,
    .fini = NULL
};
