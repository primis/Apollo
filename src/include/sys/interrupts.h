#ifndef __SYS_INTERRUPTS_H
#define __SYS_INTERRUPTS_H

#include <stdint.h>
// Pointers are generally the same size as the CPU registers...
typedef __UINTPTR_TYPE__ reg_t;

// Dynamic function jumps. Very bad stuff
typedef void (*voidCallback)(int, reg_t*);

void interruptHandlerRegister(int num, voidCallback handler);
void unhandledInterrupt(int argc, reg_t *argv);
extern voidCallback interruptHandlers[];

#endif // __SYS_INTERRUPTS_H
