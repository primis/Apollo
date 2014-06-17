/*
** <sys/interrupt.h>
** Allows the registration of interrupt vectors in the kernel space
*/
#ifndef _SYS_INTERRUPT_H
#define _SYS_INTERRUPT_H

#include <sys/callback.h>
void interruptHandlerRegister(unsigned char _n, void_callback_arg_t _f);

#endif
