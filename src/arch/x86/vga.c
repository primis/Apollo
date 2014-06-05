#include <sys/x86/ports.h>
#include <sys/screen.h>

unsigned short *vgaTextBuffer;
unsigned short vgaAttribute;
unsigned char  vgaCursorX,   vgaCursorY;
unsigned short vgaIndexPort, vgaDataPort;

void vgaScroll()
{
	unsigned int blank = ' ' | (0x0F << 8);
	if(vgaCursorY >= 25) {
		int i;
		for(i = 1; i < 24*80; i++) {
			vgaTextBuffer[i] = vgaTextBuffer[i+80];
		}
		for(i = 24*80; i<25*80; i++) {
			vgaTextBuffer[i] = blank;
		}
		vgaCursorY = 24;
	}
}

static void vgaMoveCursor(unsigned char _x, unsigned char _y)
// Move the Hardware cursor when we move text.
{
	unsigned short cursorLocation = (_y * 80) + _x;
	outb(vgaIndexPort, 14);
	outb(vgaDataPort, cursorLocation>>8);
	outb(vgaIndexPort, 15);
	outb(vgaDataPort, cursorLocation);
	vgaCursorX = _x;
	vgaCursorY = _y;
}

void vgaClear()
{
	unsigned int blank = ' ' | (0x0F << 8);
	int i;
	for(i = 1; i<25*80; i++) {
		vgaTextBuffer[i] = blank;
	}
	vgaMoveCursor(0,0);
}

void vgaChangeAttribute(char _attribute)
{
	vgaAttribute = _attribute << 8;
}

void vgaPutChar(char _c)
{
	unsigned short *location;
	if (_c == 0x08 && vgaCursorX) {		// Backspace
		vgaCursorX--;
		vgaPutChar(' ');
		vgaCursorX--;
	} else if (_c == 0x9) {				// Tabstop
		vgaCursorX = (vgaCursorX + 8) & ~(7);
	} else if (_c == '\r') {			// Carriage Return
		vgaCursorX = 0;
	} else if (_c == '\n') {			// Newline
		vgaCursorY++;
		vgaCursorX = 0;
	} else if (_c == '\f') {			// Form feed. (clear screen)
		vgaClear();
	} else if (_c >= ' ') {
		location = vgaTextBuffer + vgaCursorX + (vgaCursorY * 80);
		*location = (unsigned short)_c | vgaAttribute;
		vgaCursorX++;
	}
	if(vgaCursorX >= 80) {
		vgaPutChar('\n');
	}
	vgaScroll();
	vgaMoveCursor(vgaCursorX, vgaCursorY);
}

void vgaInit(unsigned short _data, unsigned short _idx, unsigned int _p)
{
	vgaDataPort		= _data;
	vgaIndexPort	= _idx;
	vgaAttribute	= 0x03 << 8;
	vgaTextBuffer 	= (unsigned short*)_p;
	vgaClear();
	scrInit("VGA", vgaPutChar);
}
