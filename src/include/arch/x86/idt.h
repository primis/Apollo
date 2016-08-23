/*
 * (c) 2016 Apollo Project Developers
 * <arch/x86/idt.h> - Headers for idt.c
 */

#ifndef __ARCH_X86_IDT_H
#define __ARCH_X86_IDT_H


#include <stdint.h> // Types
#include <string.h> // memset
#include <sys/interrupts.h>

extern void idtFlush(uint32_t ptr);
extern void isr0();
extern void isr1();

// Sorted by what pushed them
struct registers {
    reg_t eax, ecx, edx, esi, edi, ebx, ebp;
    reg_t ds;                           // Pushed manually
    reg_t intNum, errCode;              // ISR stub
    reg_t eip, cs, eflags, useresp, ss; // INT itself
};

typedef struct {
    uint16_t baseLow;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  typeAttr;
    uint16_t baseHigh;
} __attribute__((packed)) IDTEntry;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) IDTPointer;

void idtInit();
void encodeIDTEntry(IDTEntry *ptr, uint32_t base, uint16_t sel, uint8_t flags);
void isrHandler(struct registers *regs);

#define IDT_PRES(x)     ((x) << 0x07)   // Is the entry actually there?
#define IDT_STORE(x)    ((x) << 0x04)   // Is this a data/code segment?
#define IDT_PRIV(x)     ((x) << 0x05)   // CPU Ring Level

#define IDT_GATE_TASK32 0x05
#define IDT_GATE_INT16  0x06
#define IDT_GATE_TRAP16 0x07
#define IDT_GATE_INT32  0x0E
#define IDT_GATE_TRAP32 0x0F

#define IDT_INT32_PL0 IDT_PRES(1)   | IDT_STORE(1)  | IDT_PRIV(0) | \
        IDT_GATE_INT32

#define IDT_TRAP32_PL0 IDT_PRES(1)  | IDT_STORE(1)  | IDT_PRIV(0) | \
        IDT_GATE_TRAP32

#define IDT_TASK32_PL0 IDT_PRES(1)  | IDT_STORE(1)  | IDT_PRIV(0) | \
        IDT_GATE_TASK32


#endif //__ARCH_X86_IDT_H
