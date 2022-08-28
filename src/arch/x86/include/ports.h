#ifndef __ARCH_X86_PORTS_H
#define __ARCH_X86_PORTS_H

#include <stdint.h>

uint32_t read_cr0();
uint32_t read_cr2();
uint32_t read_cr3();
void write_cr0(uint32_t);
void write_cr2(uint32_t);
void write_cr3(uint32_t);
#endif
