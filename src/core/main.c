#include <sys/init.h>
#include  <sys/screen.h>

void initModules()
{
	scrInit();
}

int main()
{
	initModules();
	scrWrite("Hello, World!");
	return 0xDEADBEEF;
}
