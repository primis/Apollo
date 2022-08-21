/*
 * (C) 2018 Apollo Project Developers
 * For terms, see LICENSE
 * irq.c - Interrupt Request Lines (hardware)
 */

#include <sys/hal.h>
#include <sys/irqchip.h>
#include <sys/resource.h>

extern int i8259_init(irqchip_t*, int);

static resource_t at_pic0_resource = {
    .name   = "irqchip/at_pic0",
    .start  = 0x20,
    .end    = 0x21,
    .flags  = RESOURCE_IO | RESOURCE_IO_SLOW | RESOURCE_WIDTH_8,
    .parent = NULL,
    .child  = NULL
};

static resource_t at_pic1_resource = {
    .name   = "irqchip/at_pic1",
    .start  = 0xA0,
    .end    = 0xA1,
    .flags  = RESOURCE_IO | RESOURCE_IO_SLOW | RESOURCE_WIDTH_8,
    .parent = NULL,
    .child  = NULL
};

static irqchip_t at_pic0, at_pic1;

int init_irq()
{
    int i, ret;

    at_pic0_resource.child = &at_pic1_resource;
    at_pic1_resource.parent = &at_pic0_resource;

    at_pic0.data = (uintptr_t)&at_pic0_resource;
    at_pic1.data = (uintptr_t)&at_pic1_resource;  

    at_pic0.isr_start = 32;
    at_pic1.isr_start = 40;

    ret = i8259_init(&at_pic0, 0x4);
    ret |= i8259_init(&at_pic1, 0x2);
    
    // Mask all interrupts
    for(i = 32; i < 48; i++) {
        irqchip_mask(i);
    }
    // Unmask the cascade port for PIC 2
    irqchip_unmask(34);

    return ret;
}

