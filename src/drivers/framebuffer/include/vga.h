#ifndef __VGA_H
#define __VGA_H

#include <sys/resource.h>

#define VGA_BASE_POINTER_1 0xA0000
#define VGA_BASE_POINTER_2 0xB0000
#define VGA_BASE_POINTER_3 0xB8000

// Sequencer Registers
#define VGA_RESET_REG        0x00
#define VGA_CLOCKING_MODE    0x01
#define VGA_MAP_MASK         0x02
#define VGA_CHAR_MASK        0x03
#define VGA_SEQ_MEM_MODE     0x04

// Graphics Registers
#define VGA_SET_RESET        0x00
#define VGA_ENABLE_SET_RESET 0x01
#define VGA_COLOR_COMPARE    0x02
#define VGA_DATA_ROTATE      0x03
#define VGA_READ_MAP_SELECT  0x04
#define VGA_GRAPHICS_MODE    0x05
#define VGA_MISC_GRAPHICS    0x06
#define VGA_COLOR_DONT_CARE  0x07
#define VGA_BIT_MASK_REG     0x08

// CRTC Registers
#define VGA_MAX_SCAN_LINE    0x09
#define VGA_CURSOR_START     0x0A
#define VGA_CURSOR_END       0x0B
#define VGA_CURSOR_HIGH_BYTE 0x0E
#define VGA_CURSOR_LOW_BYTE  0x0F

// Register Index Offsets
#define VGA_ATTRIBUTE_REG_OFFSET 0x00
#define VGA_SEQUENCER_REG_OFFSET 0x04
#define VGA_GRAPHICS_REG_OFFSET  0x0E
#define VGA_CRTC_REG_OFFSET      0x14

typedef struct {
    uint16_t h_res, v_res;

    resource_t *vga_base, *vga_control_base, *framebuffer;
    resource_t attribute_control, sequencer, graphics, crtc;
} vga_state_t;

#endif
