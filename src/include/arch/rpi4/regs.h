/*
 * (c) 2022 Apollo Project Developers
 * <arch/rpi4/regs.h> -  Register definitions
 */

#ifndef __ARCH_RPI4_REGS_H
#define __ARCH_RPI4_REGS_H

#include <stdint.h>

typedef struct regs {
  uint64_t x0, x1, x2, x3, x4, x5, x6, x7;
  uint64_t x8, x9, x10, x11, x12, x13, x14, x15;
  uint64_t x16, x17, x18, x19, x20, x21, x22, x23;
  uint64_t x24, x25, x26, x27, x28, x29, x30, sp;
} rpi4_regs_t;

#endif
