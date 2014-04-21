#ifndef __SYS_SCREEN_H
#define __SYS_SCREEN_H

#include <sys/init.h>

// x86
#ifdef X86_MASTER_RACE
#define scrInit() x86vgaInit()
#define scrPutChar(c) x86vgaPutChar(c)
void x86vgaInit(void);
void x86vgaPutChar(char);
#endif

void scrWrite(const char*);

#endif
