#include <sys/x86/ports.h>
#define VGA_INDEX_PORT 	0x3D4
#define VGA_DATA_PORT 	0x3D5

unsigned short *x86vgaTextBuffer;
unsigned short attribute;
unsigned char cursorX, cursorY;

void x86vgaScroll()
{
	unsigned int blank = ' ' | (0x0F << 8);
	if(cursorY >= 25) {
		int i;
		for(i = 1; i < 24*80; i++) {
			x86vgaTextBuffer[i] = x86vgaTextBuffer[i+80];
		}
		for(i = 24*80; i<25*80; i++) {
			x86vgaTextBuffer[i] = blank;
		}
		cursorY = 24;
	}
}

static void x86vgaMoveCursor(unsigned char _x, unsigned char _y)
// Move the Hardware cursor when we move text.
{
	unsigned short cursorLocation = (_y * 80) + _x;
	outb(VGA_INDEX_PORT, 14);
	outb(VGA_DATA_PORT, cursorLocation>>8);
	outb(VGA_INDEX_PORT, 15);
	outb(VGA_DATA_PORT, cursorLocation);
	cursorX = _x;
	cursorY = _y;
}

void x86vgaClear()
{
	unsigned int blank = ' ' | (0x0F << 8);
	int i;
	for(i = 1; i<25*80; i++) {
		x86vgaTextBuffer[i] = blank;
	}
	x86vgaMoveCursor(0,0);
}

void x86vgaChangeAttribute(char _attribute)
{
	attribute = _attribute << 8;
}

void x86vgaPutChar(char _c)
{
	unsigned short *location;
	if (_c == 0x08 && cursorX) {		// Backspace
		cursorX--;
		x86vgaPutChar(' ');
		cursorX--;
	} else if (_c == 0x9) {				// Tabstop
		cursorX = (cursorX + 8) & ~(7);
	} else if (_c == '\r') {			// Carriage Return
		cursorX = 0;
	} else if (_c == '\n') {			// Newline
		cursorY++;
		cursorX = 0;
	} else if (_c == '\f') {			// Form feed. (clear screen)
		x86vgaClear();
	} else if (_c >= ' ') {
		location = x86vgaTextBuffer + cursorX + (cursorY * 80);
		*location = (unsigned short)_c | attribute;
		cursorX++;
	}
	if(cursorX >= 80) {
		x86vgaPutChar('\n');
	}
	//x86vgaScroll();
	x86vgaMoveCursor(cursorX, cursorY);
}

void x86vgaInit()
{
	attribute = 0x03 << 8;
	x86vgaTextBuffer = (unsigned short*)0xB8000;
	x86vgaClear();
}
