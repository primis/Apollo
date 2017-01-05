/*
 * (c) 2017 Apollo Project Developers
 * console.c - Early Kernel mode console for debugging
 * We use VGA cause it's easy, though this can be anything
 * that can output a charecter set
 */

#include <arch/x86/vga.h>
#include <arch/x86/ports.h>

static char cursorY;
static char cursorX;
unsigned short *VGAPointer;

static void move_cursor()
{
    unsigned short cursorLocation = cursorY * 80 + cursorX;
    outb(VGA_INDEX_3, VGA_CURSOR_HIGH_BYTE);
    outb(VGA_INDEX_3+1, cursorLocation >> 8);
    outb(VGA_INDEX_3, VGA_CURSOR_LOW_BYTE);
    outb(VGA_INDEX_3+1, cursorLocation);
}

static void scroll()
{
    // White on Black
    unsigned char attributeByte = (0 << 4) | (15 & 0x0f);
    unsigned short blank = 0x20 | (attributeByte << 8);
    if(cursorY >= 25) {
        int i;
        for(i=0; i<24*80; i++) {
            VGAPointer[i] = VGAPointer[i+80];
        }
        for(i=24*80; i<25*80; i++) {
            VGAPointer[i] = blank;
        }
        cursorY = 24;
    }
}

void console_put(char c)
{
    unsigned char backColor = 0;
    unsigned char foreColor = 15;

    unsigned char attributeByte = (backColor << 4) | (foreColor & 0x0f);
    unsigned short attribute = attributeByte << 8;
    unsigned short *location;

    if (c == 0x08 && cursorX) {
        cursorX--;      // It's backspace
    } else if (c == 0x09) {
        cursorX = (cursorX+8) & ~(8-1); // Tab handling
    } else if (c == '\r') {
        cursorX = 0;    // Carraige return
    } else if (c == '\n') {
        cursorX = 0;    // New Line
        cursorY++;
    } else if(c >= ' ') {
        location = VGAPointer + (cursorY*80 + cursorX);
        *location = c | attribute;
        cursorX++;
    }
    if (cursorX >= 80){
        cursorX = 0;
        cursorY++;
    }
    scroll();
    move_cursor();
}
void console_clear()
{
    int i;
    for(i=0; i<26; i++) {
        console_put('\n');
    }
    cursorY = 0;
    cursorX = 0;
    move_cursor();
}

void consoleInit()
{
    VGAPointer = (unsigned short *)VGA_BASE_POINTER;
    console_clear();
}
