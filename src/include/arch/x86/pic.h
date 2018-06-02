/*
 * (C) 2018 Apollo Project Developers
 * pic.h - Definitions for controlling the PIT microcontrollers
 */

#ifndef __ARCH_X86_PIC_H
#define __ARCH_X86_PIC_H

// PIC Commands

#define PIC_ICW1_ICW4       0x01    // ICW4 not needed (ICW4 Needed if set)
#define PIC_ICW1_SINGLE     0x02    // Single cascade mode
#define PIC_ICW1_INTERVAL4  0x04    // Call address interval 4 (8 if not set)
#define PIC_ICW1_LEVEL      0x08    // Level triggered mode (Edge if not set)
#define PIC_ICW1_INIT       0x10    // Initialize PIC
#define PIC_ICW1_MCS_IVA(ad) (ad << 5) // MCS Interrupt Vector Address

#define PIC_ICW4_8086       0x01    // 8086/88 Mode (MCS-80/58 if not set)
#define PIC_ICW4_AUTO       0x02    // Auto EOI (Manual EOI if not set)
#define PIC_ICW4_BUFFERED   0x04    // Buffered mode
#define PIC_ICW4_MASTER     0x08    // Master Mode (Slave if not set)
#define PIC_ICW4_NESTED     0x10    // Special Fully Nested Mode

// OCW 2 & 3 Commands, OCW1 is written/read by the data register
#define PIC_ISR_SELECT      0x01    // Select ISR (or IRR if 0) - OCW3
#define PIC_READ_REGISTER   0x02    // Read Register Command - OCW3
#define PIC_OCW3_SELECT     0x08    // Select the OCW3 register (OCW2 otherwise)
#define PIC_EOI             0x20    // End of interrupt - OCW2
#define PIC_SPECIFIC        0x40    // Specific Modifier - IR in bits 0-2
#define PIC_ROTATE          0x80    // Rotate in auto EOI mode
#define PIC_SET_PRIORITY    0xC0    // Set priority - IR in bits 0-2

#define PIC_ROTATE_SP   PIC_ROTATE  | PIC_EOI | PIC_SPECIFIC
#define PIC_ROTATE_NSP  PIC_ROTATE  | PIC_EOI
#define PIC_EOI_SP      PIC_EOI     | PIC_SPECIFIC
#define PIC_READ_ISR    PIC_READ_REGISTER | PIC_OCW3_SELECT | PIC_ISR_SELECT
#define PIC_READ_IRR    PIC_READ_REGISTER | PIC_OCW3_SELECT


void PIC_init(unsigned char icw1, unsigned char icw3, unsigned char icw4,
        int offset, unsigned short cmd);

void PIC_set_mask(unsigned char line, unsigned short data);
void PIC_clear_mask(unsigned char line, unsigned short data);
void PIC_sendEOI(unsigned short command);
unsigned char PIC_get_IRR(unsigned short command);
unsigned char PIC_get_ISR(unsigned short command);

#endif
