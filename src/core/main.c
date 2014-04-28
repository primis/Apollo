#include <sys/init.h>
#include <sys/screen.h>
#include <stdarg.h>
void initModules(char c,...)
{
	scrInit();
	gdtInit();
}

int main()
{
	initModules(' ');
	scrWrite("Hello, Netbooting World! :D");
	return 0xDEADC0DE;
}
