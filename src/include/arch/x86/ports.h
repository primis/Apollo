#ifndef __ARCH_X86_PORTS_H
#define __ARCH_X86_PORTS_H

#include <stdint.h>

#define IRQ(n) (n+32)

typedef struct {
    int reg;
    uint8_t cmd;
} device_init_t;

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
void iowait(void);
uint8_t read_register(int base, int reg);
void write_register(int base, int reg, uint8_t value);
uint32_t read_cr0();
uint32_t read_cr2();
uint32_t read_cr3();
void write_cr0(uint32_t);
void write_cr2(uint32_t);
void write_cr3(uint32_t);
#endif
