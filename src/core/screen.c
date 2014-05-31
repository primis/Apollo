#include <sys/screen.h>

void scrWrite(const char *_str)
{
	while(*_str) {
		scrPutChar((int)*_str++);
	}
}

void scrWriteHex(unsigned int n)
{
	int i, temp;
	char noZ = 1;	// Boolean to see if we have any Preceding zeros.

	scrWrite("0x");	// Bit of formatting so we know it's hexadecimal.

	for(i = 28; i>0; i -= 4) {
		temp = (n >> i) & 0xF;				// Base Shift 10 -> 16
		if (temp == 0 && noZ != 0) {
			continue;	// We don't want leading Zeros, Ignore it.
		} else if (temp >= 0xA) {			// A-F
			noZ = 0;	// Number has a Sig-Dig, Don't ignore 0 anymore.
			scrPutChar(temp - 0xA + 'A');	// Ascii shift.
		} else {		// 0-9
			noZ = 0;						// No longer a leading zero.
			scrPutChar(temp + '0');
		}
	}

	// That loop isn't perfect, the last digit won't get displayed. This
	// Is the "edge case" fix here.
	temp = n & 0xF;
	if (temp >= 0xA) {
		scrPutChar(temp - 0xA + 'A');
	} else {
		scrPutChar(temp + '0');
	}
}

