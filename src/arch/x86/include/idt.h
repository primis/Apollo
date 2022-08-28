/*
 * (c) 2022 Apollo Project Developers
 * idt.h - Headers for idt.c
 */

#ifndef __IDT_H
#define __IDT_H

#include <stdint.h> // Types
#include <string.h> // memset
#include <sys/interrupts.h>

extern void (*ack_irq)(unsigned);
extern void (*enable_irq)(uint8_t, unsigned);

typedef struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed)) idt_entry_t;

typedef struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

#define IDT_PRES(x)     ((x) << 0x07)   // Is the entry actually there?
#define IDT_STORE(x)    ((x) << 0x04)   // Is this a data/code segment?
#define IDT_PRIV(x)     ((x) << 0x05)   // CPU Ring Level

#define IDT_GATE_TASK32 0x05
#define IDT_GATE_INT16  0x06
#define IDT_GATE_TRAP16 0x07
#define IDT_GATE_INT32  0x0E
#define IDT_GATE_TRAP32 0x0F

#define IDT_INT32_PL0 IDT_PRES(1)   | IDT_STORE(0)  | IDT_PRIV(0) | \
        IDT_GATE_INT32

#define IDT_TRAP32_PL0 IDT_PRES(1)  | IDT_STORE(0)  | IDT_PRIV(0) | \
        IDT_GATE_TRAP32

#define IDT_TASK32_PL0 IDT_PRES(1)  | IDT_STORE(0)  | IDT_PRIV(0) | \
        IDT_GATE_TASK32

#endif
