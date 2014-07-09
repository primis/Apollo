#include <sys/screen.h>
#include <sys/x86/idt.h>
#include <sys/x86/ports.h>

extern void_callback_arg_t interruptHandlers[];

unsigned short masterPIC, slavePIC;

void setIRQMask(int _IRQ)
{
	unsigned short port;
	unsigned char  mask;

	if(_IRQ < 8) {
		port = 0x21;
	} else {
		port = 0xA1;
		_IRQ -= 8;
	}
	mask = inb(port) | (1 << _IRQ);
	outb(port, mask);
}

void clearIRQMask(int _IRQ)
{
	unsigned short port;
	unsigned char  mask;

	if(_IRQ < 8) {
		port = 0x21;
	} else {
		port = 0xA1;
		_IRQ -= 8;
	}
	mask = inb(port) & ~(1 << _IRQ);
	outb(port, mask);
}

void picInit(unsigned short _pic1, unsigned short _pic2)
{
	masterPIC = _pic1;
	slavePIC  = _pic2;

	unsigned short data1 = _pic1 + 1;
	unsigned short data2 = _pic2 + 1;

	outb(_pic1, 0x10 + 0x01);	// Init + ICW4 (Set up Cascade mode)
	outb(_pic2, 0x10 + 0x01);

	// After this init, the PICs are expecting three additional pieces
	// of data; Vector offset, Cascade position, and mode.

	outb(data1, 0x20);	// Relocate pic1 to interrupt 0x20
	outb(data2, 0x28);	// Relocate pic2 to interrupt 0x28

	outb(data1, 4);		// Tell Master it has Slave on line 2 (00000100)
	outb(data2, 2);		// Tell the Slave it's cascading      (00000010)

	outb(data1, 1);		// Tell the Pic it's in 8086 mode
	outb(data2, 1);

	// At this point, init is complete, and the pic goes into normal
	// Operating mode. But there is still one last thing to do, Set the
	// Interrupt Masks so we don't get interrupts until we set up the
	// Devices on their respective IRQ Lines.

	outb(data1, 0xFF);	// Lines 0-7 are all masked
	outb(data2, 0xFF);	// Lines 8-15 are now masked too.
}

void irqHandler(struct regs *_r)
{
	if (_r->int_no >= 40) {		// Is this from the slave pic?
		outb(slavePIC, 0x20);	// Send Acknowledgement to the Slave
	}
	outb(masterPIC, 0x20);		// Send Acknowledgement to the Master

	void_callback_arg_t handler = interruptHandlers[_r->int_no];
	handler(_r->int_no, _r->err_code, _r->eax, _r->ebx, _r->ecx, _r->edx,
		_r->edi, _r->esi);
}

