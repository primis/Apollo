/*
 * <sys/init.h>
 * The init functions of everything in one file
 */

#ifndef _SYS_INIT_H
#define _SYS_INIT_H
#include <sys/screen.h>
#include <time.h>
void gdtInit(void);
void idtInit(void);
void vgaInit(unsigned short data, unsigned short index, unsigned int base);
void pitInit(unsigned int frequency);
void picInit(unsigned short a, unsigned short b);

#endif
