#ifndef _SYS_X86_PORTS_H
#define _SYS_X86_PORTS_H
/* This is not the documentation, if you want to know what functions do,
** Please read the api documentation!
*/
void outb(unsigned short, unsigned char);
unsigned char inb(unsigned short);
unsigned short inw(unsigned short);
void NMIEnable(void);
void NMIDisable(void);
void cli(void);
void sti(void);
#endif
