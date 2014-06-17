#include <sys/init.h>
#include <sys/screen.h>
void initModules()
{
	vgaInit(0x3D5, 0x3D4, 0xB8000);		// Index, Data, Base Address.
	gdtInit();							// Grand Descriptor Table
	scrWrite("Initializing The IDT\t\t\t");
	idtInit();							// Init Interrupts
	scrWrite("[ OK ]\n");
	scrWrite("Initializing The PIC\t\t\t");
	picInit(0x20, 0xA0);				// Command Registers of the PIC's
	scrWrite("[ OK ]\n");
}

int main()
{
	initModules();
	scrWrite("Hello, Netbooting World! :D\n");
	asm volatile ("int 0x3");
	return 0xDEADC0DE;
}
