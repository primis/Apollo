/*
 * (c) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * 8259_pic.c - Intel Programmable Interrupt Controller driver
 *
 * Datasheet available here:
 * https://pdos.csail.mit.edu/6.828/2018/readings/hardware/8259A.pdf
 */

#include <sys/irqchip.h>
#include <sys/resource.h>

#include "include/8259_pic.h"

/**
 ** Generic PIC driver code
 **/

static int i8259_ack(irqchip_t *chip, uint32_t n)
{
    resource_t *base;
    uint8_t cmd;

    if(chip == NULL)
    {
        return -1;
    }
    base = (resource_t *)chip->data;

    cmd = PIC_EOI;

    /* Check to see if we're a cascaded PIC, if so, run an EOI against the 
     * main PIC too 
     */
    if(chip->flags & IRQCHIP_CASCADE_SECONDARY)
    {
        if(resource_write(&cmd, base->parent, 0, 1))
        {
            return -1;
        }
    }

    /* TODO: Spurrious interrupt checks */
    /* Send the EOI to the PIC */
    if(resource_write(&cmd, base, 0, 1))
    {
        return -1;
    }

    return 0;
}

static int i8259_mask(irqchip_t *chip, uint32_t n)
{

    uint8_t mask;
    resource_t *base;

    if(chip == NULL)
    {
        return -1;
    }
    base = (resource_t *)chip->data;

    /* Retrieve existing mask from the Interrupt Mask Register.
     * The IMR, set via OCW1, is described in Fig. 8 of the data sheet.
     */
    if(resource_read(&mask, base, 1, 1))
    {
        return -1;
    }

    /* Mask the desired IRQ line by setting that bit into the mask.
     */
    mask |= (1 << n);

    /*
     * Push the mask out back to the IMR (OCW1).
     */
    if(resource_write(&mask, base, 1, 1))
    {
        return -1;
    }
    return 0;
}

static int i8259_unmask(irqchip_t *chip, uint32_t n)
{
    uint8_t mask;
    resource_t *base;

    if(chip == NULL)
    {
        return -1;
    }
    base = (resource_t *)chip->data;

    /* Retrieve existing mask from the Interrupt Mask Register.
     * The IMR, set via OCW1, is described in Fig. 8 of the data sheet.
     */
    if(resource_read(&mask, base, 1, 1))
    {
        return -1;
    }

    /* Mask the desired IRQ line by clearing that bit in the mask.
     */
    mask &= ~(1 << n);

    /*
     * Push the mask out back to the IMR (OCW1).
     */
    if(resource_write(&mask, base, 1, 1))
    {
        return -1;
    }
    return 0;
}

int i8259_init(irqchip_t *chip, uint8_t icw3)
{
    uint8_t mask;
    uint8_t cmd;
    resource_t *base; 
    int cascade_mode = 0;

    /* Quick null pointer check */
    if(chip == NULL)
    {
        return -1;
    }

    base = (resource_t *)chip->data;
    if(base == NULL)
    {
        return -1;
    }

    /* Make sure that the interrupt range requested is within the 8 bit limit */
    if(chip->isr_start + 8 > 255)
    {
        return -1;
    }

    /* Check to see if we need ICW3 or not */
    if((chip->flags & IRQCHIP_CASCADE_PRIMARY) ||
       (chip->flags & IRQCHIP_CASCADE_SECONDARY))
    {
        cascade_mode = 1;
    }

    if(resource_read(&mask, base, 1, 1))
    {
        return -1;
    }

    /* ICW1 is described in Fig. 7 of the datasheet.
     * We want:
     * - Begin initialization
     * - ICW4 needed
     * - Cascade mode is optional depending on topology
     * - Edge Triggered Interrupts (Default)
     * 
     * Other bits are only useful for MCS-80/85 mode
     */
    cmd = PIC_ICW1_INIT | PIC_ICW1_ICW4; 
    if (cascade_mode == 0)
    {
        cmd |= PIC_ICW1_SINGLE; /* Disable Cascade mode */
    }
    if(resource_write(&cmd, base, 0, 1))
    {
        return -1;
    }

    /* ICW2 is also described in Fig. 7 of the datasheet.
     * This represents the vector address of the PIC chip in 8086 mode.
     */ 
    cmd = chip->isr_start;
    if(resource_write(&cmd, base, 1, 1))
    {
        return -1;
    }

    /* ICW3 is also described in Fig. 7 of the datasheet.
     * It isn't written in single PIC mode, so we clause guard for that
     * ICW3 is passed from the calling function, as the syntax changes
     * depending if the chip is set up as the primary PIC or a cascaded one.
     * There is no way to tell programmatically which, as that is set via a 
     * hardware pin on the PIC itself.
     */
    if(cascade_mode == 1) {
        if(resource_write(&icw3, base, 1, 1)) 
        {
            return -1;
        }
    }

    /* ICW4 is also described in Fig. 7 of the datasheet.
     * ICW4 determines the operating mode of the PIC.
     * We want:
     * - 8086/8088 Mode (not MCS-80/85 mode)
     * - Normal EOI Mode (default, otherwise auto EOI)
     * - Non-Buffered Mode (default)
     * - Not Special fully nested mode (default)
     */
    cmd = PIC_ICW4_8086;
    if(resource_write(&cmd, base, 1, 1))
    {
        return -1;
    }

    /* Finally, Restore the mask cached earlier */
    if(resource_write(&mask, base, 1, 1))
    {
        return -1;
    }

    /* The 8259 has 8 IRQ Channels */
    chip->isr_len = 8;
    chip->ack = &i8259_ack;
    chip->mask = &i8259_mask;
    chip->unmask = &i8259_unmask;

    irqchip_register(chip);

    return 0;
}
