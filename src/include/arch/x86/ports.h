#ifndef __ARCH_X86_PORTS_H
#define __ARCH_X86_PORTS_H

void outb(unsigned short port, unsigned char value);
unsigned char inb(unsigned short port);
unsigned short inw(unsigned short port);
void iowait(void);
#endif
