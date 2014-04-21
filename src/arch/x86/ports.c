#include <sys/x86/ports.h>

void outb(unsigned short _port, unsigned char _data)
{
	asm volatile("outb %1, %0" : : "dN" (_port), "a" (_data));
}

unsigned char inb(unsigned short _port)
{
	unsigned int data;
	asm volatile("inb %1, %0" : "=a" (data) : "dN" (_port));
	return data;
}

unsigned short inw(unsigned short _port)
{
	unsigned short data;
	asm volatile("inw %1, %0" : "=a" (data) : "dN" (_port));
	return data;
}

void NMIEnable(void)
{
	outb(0x70, inb(0x70)&0x7F);
}

void NMIDisable(void)
{
	outb(0x70, inb(0x70)|0x80);
}

void cli(void)
{
	asm volatile("cli");
}
void sti(void)
{
	asm volatile("sti");
}
