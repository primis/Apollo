/*
 * (c) 2024 Apollo Project Developers
 * <arch/x86/regs.h> -  Register definitions
 */

#ifndef __ARCH_X86_REGS_H
#define __ARCH_X86_REGS_H

#include <stdint.h>

typedef struct regs {
  uint32_t ds;
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  uint32_t interrupt_num, error_code;
  uint32_t eip, cs, eflags, useresp, ss;
} x86_regs_t;

uint32_t read_cr0();
uint32_t read_cr2();
uint32_t read_cr3();
void write_cr0(uint32_t);
void write_cr2(uint32_t);
void write_cr3(uint32_t);

#endif
