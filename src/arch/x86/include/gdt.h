/*
 * (c) 2022 Apollo Project Developers
 * gdt.h - x86 32 Bit GDT definitions.
 */

#ifndef __ARCH_X86_GDT_H
#define __ARCH_X86_GDT_H

#include <stdint.h>

typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  type : 4;
    uint8_t  s    : 1;            /* 's' should always be 1, except for */
    uint8_t  dpl  : 2;            /* the NULL segment. */
    uint8_t  p    : 1;
    uint8_t  limit_high : 4;
    uint8_t  avail: 1;
    uint8_t  l    : 1;
    uint8_t  d    : 1;
    uint8_t  g    : 1;
    uint8_t  base_high;
} gdt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

typedef struct {
    uint32_t prev_tss;
    uint32_t esp0, ss0, esp1, ss1, esp2, ss2;
    uint32_t cr3, eip, eflags;
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint16_t trap, iomap_base;
} tss_entry_t;

extern void gdtFlush();

#define TY_CODE 8

/* Applies to code segments */
#define TY_CONFORMING 4
#define TY_READABLE 2

/* Applies to data segments. */
#define TY_DATA_EXPAND_DIRECTION 4
#define TY_DATA_WRITABLE 2

/* Applies to both; set by the CPU. */
#define TY_ACCESSED 1



/*
 * Here There Be Macros... Y'arr
 *
 * Each define is for a specific flag in the descriptor field.
 * Refer to the intel documentation for a description of what each is.
 */

#define SEG_DESCTYPE(x)  ((x) << 0x04) // Descriptor type
#define SEG_PRES(x)      ((x) << 0x07) // Present
#define SEG_SAVL(x)      ((x) << 0x0C) // Available for system use
#define SEG_LONG(x)      ((x) << 0x0D) // Long mode
#define SEG_SIZE(x)      ((x) << 0x0E) // Size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x)      ((x) << 0x0F) // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
#define SEG_PRIV(x)     (((x) &  0x03) << 0x05)   // Set privilege level (0 - 3)

#define SEG_DATA_RD        0x00 // Read-Only
#define SEG_DATA_RDA       0x01 // Read-Only, accessed
#define SEG_DATA_RDWR      0x02 // Read/Write
#define SEG_DATA_RDWRA     0x03 // Read/Write, accessed
#define SEG_DATA_RDEXPD    0x04 // Read-Only, expand-down
#define SEG_DATA_RDEXPDA   0x05 // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD  0x06 // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX        0x08 // Execute-Only
#define SEG_CODE_EXA       0x09 // Execute-Only, accessed
#define SEG_CODE_EXRD      0x0A // Execute/Read
#define SEG_CODE_EXRDA     0x0B // Execute/Read, accessed
#define SEG_CODE_EXC       0x0C // Execute-Only, conforming
#define SEG_CODE_EXCA      0x0D // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC     0x0E // Execute/Read, conforming
#define SEG_CODE_EXRDCA    0x0F // Execute/Read, conforming, accessed

#define GDT_CODE_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | \
                     SEG_PRIV(0)     | SEG_CODE_EXRD

#define GDT_DATA_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | \
                     SEG_PRIV(0)     | SEG_DATA_RDWR

#define GDT_CODE_PL1 SEG_DESCTYPE(1) | SEG_PRES(1) | \
                     SEG_PRIV(1)     | SEG_CODE_EXRD

#define GDT_DATA_PL1 SEG_DESCTYPE(1) | SEG_PRES(1) | \
                     SEG_PRIV(1)     | SEG_DATA_RDWR

#define GDT_CODE_PL2 SEG_DESCTYPE(1) | SEG_PRES(1) | \
                     SEG_PRIV(2)     | SEG_CODE_EXRD

#define GDT_DATA_PL2 SEG_DESCTYPE(1) | SEG_PRES(1) | \
                     SEG_PRIV(2)     | SEG_DATA_RDWR

#define GDT_CODE_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | \
                     SEG_PRIV(3)     | SEG_CODE_EXRD

#define GDT_DATA_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | \
                     SEG_PRIV(3)     | SEG_DATA_RDWR

#endif // __ARCH_X86_GDT_H
