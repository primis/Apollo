#ifndef __VGA_TEXT_H
#define __VGA_TEXT_H

#define VGA_BASE_POINTER 0xB8000    // TODO: These are specific, 
#define VGA_INDEX_1 0x3C4           // consider moving them to a config!
#define VGA_INDEX_2 0x3CE
#define VGA_INDEX_3 0x3D4

#define VGA_CURSOR_HIGH_BYTE 14
#define VGA_CURSOR_LOW_BYTE 15

// VGA Colors
#define VGA_C_BLACK           0
#define VGA_C_BLUE            1
#define VGA_C_GREEN           2
#define VGA_C_CYAN            3
#define VGA_C_RED             4
#define VGA_C_MAGENTA         5
#define VGA_C_BROWN           6
#define VGA_C_LIGHTGRAY       7
#define VGA_C_DARKGRAY        8
#define VGA_C_LIGHTBLUE       9
#define VGA_C_LIGHTGREEN     10
#define VGA_C_LIGHTCYAN      11
#define VGA_C_LIGHTRED       12
#define VGA_C_LIGHTMAGENTA   13
#define VGA_C_LIGHTBROWN     14
#define VGA_C_WHITE          15

#endif
