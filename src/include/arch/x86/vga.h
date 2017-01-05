#ifndef __ARCH_X86_VGA_H
#define __ARCH_X86_VGA_H

#define VGA_BASE_POINTER 0xB8000
#define VGA_INDEX_1 0x3C4
#define VGA_INDEX_2 0x3CE
#define VGA_INDEX_3 0x3D4

#define VGA_CURSOR_HIGH_BYTE 14
#define VGA_CURSOR_LOW_BYTE 15
void consoleInit();
#endif
