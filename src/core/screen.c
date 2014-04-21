#include <sys/screen.h>

void scrWrite(const char *str)
{
	while(*str) {
		scrPutChar((int)*str++);
	}
}
