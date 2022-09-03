/*
 * (c) 2022 Apollo Project Developers
 * <arch/rpi4/hal.h> - Hardware abstraction layer
 */

#ifndef __ARCH_RPI4_HAL_H
#define __ARCH_RPI4_HAL_H

#include <stdint.h>
#include <sys/hal.h>

#define THREAD_STACK_SZ 0x2000 // Kernel stack 8k


typedef struct address_space {
  uint32_t *directory;
  spinlock_t lock;
} address_space_t;

static inline unsigned get_page_size() {
  return 4096;
}

static inline unsigned get_page_shift() {
  return 12;
}

static inline unsigned get_page_mask() {
  return 0xFFF;
}

static inline uintptr_t round_to_page_size(uintptr_t x) {
  if ((x & 0xFFF) != 0)
    return ((x >> 12) + 1) << 12;
  else
    return x;
}

static inline void abort() {
  for(;;);
}

#include "arch/rpi4/regs.h"

struct jmp_buf_impl {
    uint64_t si;
};

typedef struct jmp_buf_impl jmp_buf[1];

static inline void jmp_buf_set_stack(jmp_buf buf, uintptr_t stack) {
  buf[0].si = stack;
}

static inline void jmp_buf_to_regs(struct regs *r, jmp_buf buf) {
 // todo
}

#define abort() (void)0


#endif
