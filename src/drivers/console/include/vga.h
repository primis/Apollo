#ifndef __VGA_TEXT_H
#define __VGA_TEXT_H

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

#define DEFAULT_BG_COLOR VGA_C_BLUE
#define DEFAULT_FG_COLOR VGA_C_LIGHTGRAY

typedef struct {
    char     escape_buf[4];
    int      escape_buf_idx;
    int      escape_nums[4];
    int      escape_num_idx;
    uint8_t  cursor_y;
    uint8_t  cursor_x;

    device_t   *dev;
    resource_t *framebuffer;
    uint16_t    max_rows;
    uint16_t    max_cols;
    int         bg_color;
    int         fg_color;
    int         bold;
    int         in_escape;
} vga_console_state_t;

#endif
