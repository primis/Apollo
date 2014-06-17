#include <sys/x86/idt.h>
#include <string.h>
#include <sys/screen.h>

extern void idt_flush(unsigned int);
extern void_callback_arg_t interruptHandlers[];
void unhandledInterrupt(unsigned int _intNum, ...);

static void idtSetGate(unsigned char, unsigned int, unsigned short,
 unsigned char);

idt_entry_t	idt[256];
idt_ptr_t	idt_ptr;


static void idtSetGate(unsigned char n, unsigned int base,
 unsigned short selector, unsigned char flags)
{
	idt[n].base_low = base & 0xFFFF;
	idt[n].base_high= (base >> 16) & 0xFFFF;

	idt[n].selector	= selector;
	idt[n].always0	= 0;
	idt[n].flags	= flags;
}

void isrHandler(registers_t _r)
{
	void_callback_arg_t handler = interruptHandlers[_r.int_no];
	handler(_r.int_no, _r.err_code, _r.eax, _r.ebx, _r.ecx, _r.edx,
		_r.edi, _r.esi);
}

void idtInit()
{
	int i;
	unsigned int temp;

	// Get distance between isr stubs.
	temp = (unsigned int)isr1 - (unsigned int)isr0;

	idt_ptr.limit	= sizeof(idt_entry_t) * 256 - 1;
	idt_ptr.base	= (unsigned int)&idt;
	memset(&idt, 0, sizeof(idt_entry_t)*256);

	// Initialize the Vector Table with the stubs.
	for(i=0; i<128; i++) {
		idtSetGate(i, (unsigned int)isr0 + (temp*i), 0x08, 0x8E);
	}
	// Initialize the Function calls to default state
	for(i=0; i<256; i++) {
		interruptHandlerRegister(i, unhandledInterrupt);
	}
	idt_flush((unsigned int)&idt_ptr);
}
