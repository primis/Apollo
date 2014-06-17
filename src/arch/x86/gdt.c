#include <sys/x86/gdt.h>
#include <sys/screen.h>
extern void gdt_flush(unsigned);	// located in init.s

void gdtSetGate(int, unsigned long, unsigned long, unsigned char,
	unsigned char);

gdt_entry_t gdt[5];
gdt_ptr_t	gdtPointer;

void gdtInit()
{
	scrWrite("Setting up GDT...\t\t\t\t\t\t");
	gdtPointer.limit = (sizeof(gdt_entry_t)*5) - 1;
	gdtPointer.base  = (unsigned)&gdt;

	gdtSetGate(0, 0, 0, 0, 0);
	gdtSetGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	gdtSetGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
	gdtSetGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
	gdtSetGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
	gdt_flush((unsigned)&gdtPointer);
	scrWrite("[  OK  ]\n");
}

void gdtSetGate(int _n, unsigned long _base, unsigned long _limit,
	unsigned char _access, unsigned char _gran)
{
	gdt[_n].base_low	= (_base & 0xFFFF);
	gdt[_n].base_mid	= (_base >> 16) & 0xFF;
	gdt[_n].base_high	= (_base >> 24) & 0xFF;

	gdt[_n].limit_low	= (_limit & 0xFFFF);
	gdt[_n].granularity	= ((_limit >> 16) & 0x0F);
	gdt[_n].granularity |= _gran & 0xF0;
	gdt[_n].access		= _access;
}
