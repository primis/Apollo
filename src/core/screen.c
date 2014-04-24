#include <sys/screen.h>

void scrWrite(const char *_str)
{
	while(*_str) {
		scrPutChar((int)*_str++);
	}
}
