#ifndef __SYS_IRQCHIP_H
#define __SYS_IRQCHIP_H

#include <stdint.h>

#define IRQCHIP_CASCADE_PRIMARY     0x00000001 
#define IRQCHIP_CASCADE_SECONDARY   0x00000002

/**
 * struct irqchip - per irq chip information
 * @name:       Unique name of the chip (see irqchip_state)
 * 
 * @mask:       Enable an interrupt line
 * @unmask:     Disable an interrupt line
 * @ack:        Acknowledge an interrupt
 * @isr_start:  Offset from hardware interrupt numbering
 * @isr_len:    Number of interrupts serviced by this chip
 * @next:       Pointer to next irqchip 
 * @data:       Implementation specific data    
 */
typedef struct irqchip {
    const char *name;
    uint32_t flags;
        
    int (*mask)(struct irqchip *data, uint32_t n);
    int (*unmask)(struct irqchip *data, uint32_t n);
    int (*ack)(struct irqchip *data, uint32_t n);

    uint32_t isr_start;
    uint32_t isr_len;

    struct irqchip *next;
    uintptr_t data;
} irqchip_t;

/**
 * @brief Register an IRQ Chip 
 * 
 * @param c Pointer to the IRQChip instance
 * @return int value of the init() function return
 */
int irqchip_register(irqchip_t *c);

/**
 * @brief De-register an IRQ Chip 
 * 
 * @param c Pointer to the IRQChip instance
 * @return int value of the fini() function return
 */
int irqchip_unregister(irqchip_t *c);

/**
 * @brief Acknowledge an interrupt
 * 
 * @param c Pointer to the IRQChip instance
 * @param n The interrupt line to acknowledge
 */
void irqchip_ack(uint32_t n);

/**
 * @brief mask an interrupt
 * 
 * @param n The interrupt line to mask
 */
void irqchip_mask(uint32_t n);

/**
 * @brief Unmask an interrupt
 * 
 * @param n The interrupt line to unmask
 */
void irqchip_unmask(uint32_t n);

#endif /* __SYS_IRQCHIP_H */
