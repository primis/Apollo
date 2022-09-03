/*
 * (C) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * irq.c - x86 specific IRQ driver bringup
 */

#include <sys/hal.h>
#include <sys/resource.h>
#include <sys/irqchip.h>
#include <sys/device.h>

static resource_t at_pic0_resource = {
    .name   = "irqchip/at_pic0",
    .start  = 0x20,
    .end    = 0x21,
    .flags  = RESOURCE_IO | RESOURCE_IO_SLOW | RESOURCE_WIDTH_8,
    .parent = NULL,
};

static resource_t at_pic1_resource = {
    .name   = "irqchip/at_pic1",
    .start  = 0xA0,
    .end    = 0xA1,
    .flags  = RESOURCE_IO | RESOURCE_IO_SLOW | RESOURCE_WIDTH_8,
    .parent = &at_pic0_resource,
};

static irqchip_t at_pic0 = {
    .name = "at_pic0",
    .flags = IRQCHIP_CASCADE_PRIMARY,
    .isr_start = 32,
    .data = &at_pic0_resource
};

static irqchip_t at_pic1 = {
    .name = "at_pic1",
    .flags = IRQCHIP_CASCADE_SECONDARY,
    .isr_start = 40,
    .data = &at_pic1_resource
};

static int config[] = {0x4, 0x2};

static device_t at_pic0_dev = {
    .name   = "at_pic0",
    .data   = &at_pic0
};

static device_t at_pic1_dev = {
    .name   = "at_pic1",
    .data   = &at_pic1
};

int init_irq()
{
    int i, ret;

    ret = device_register(&at_pic0_dev, base_device(), "irqchip/8254_pic");
    ret |= device_register(&at_pic1_dev, base_device(), "irqchip/8254_pic");
    ret |= device_init(&at_pic0_dev, &config[0]);
    ret |= device_init(&at_pic1_dev, &config[1]);

    // Mask all interrupts
    for(i = 32; i < 48; i++) {
        irqchip_mask(i);
    }
    // Unmask the cascade port for PIC 2
    irqchip_unmask(34);

    return ret;
}
