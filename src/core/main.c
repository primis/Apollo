#include <sys/init.h>
#include <sys/screen.h>
void initModules()
{
	vgaInit(0x3D5, 0x3D4, 0xB8000);
	gdtInit();
	idtInit();
}

int main()
{
	initModules();
	scrWrite("Hello, Netbooting World! :D\n");
	asm volatile ("int 0x3");
	asm volatile ("int 34");
	return 0xDEADC0DE;
}
