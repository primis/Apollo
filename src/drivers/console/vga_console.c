/*
 * (c) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * vga_console.c - Early Kernel mode console for debugging
 * We use VGA cause it's easy, though this can be anything
 * that can output a charecter set
 */


#include <arch/x86/ports.h>
#include <string.h>
#include <sys/hal.h>
#include "include/vga.h"

static const int defaultBackColor = VGA_C_BLUE;
static const int defaultForeColor = VGA_C_LIGHTGRAY;

static char     escape_buf[4];
static int      escape_buf_idx = 0;
static int      escape_nums[4];
static int      escape_num_idx = 0;
static uint8_t  cursorY = 0;
static uint8_t  cursorX = 0;
static uint16_t *VGAPointer;

static int      backColor = defaultBackColor;
static int      foreColor = defaultForeColor;

static int      foreBold  = 0;
static int      in_escape = 0;

static void cls();

// These are inflexible in design, for full support, include VGA Display driver
void vga_move_cursor(uint16_t, uint16_t)  __attribute__((__weak__));
void vga_move_cursor(uint16_t cursorX, uint16_t cursorY) {
  return;
}

void vga_set_font(int)  __attribute__((__weak__));
void vga_set_font(int size) {
  return;
}

uint16_t vga_get_max_rows() __attribute__((__weak__));
uint16_t vga_get_max_rows() {
  return 25;
}

void* vga_get_framebuffer_base() __attribute__((__weak__));
void* vga_get_framebuffer_base() {
  return (void*) 0xB8000;
}

static void handle_colour_escape(int e) {
  switch (e) {
  case 0:
    /* Reset */
    foreColor = defaultForeColor;
    backColor = defaultBackColor;
    foreBold = 0;
    break;
  case 1:
    /* Bold */
    if (!foreBold)
      foreColor += 8;
    foreBold = 1;
    break;

  case 30: foreColor = (!foreBold) ? VGA_C_BLACK : VGA_C_DARKGRAY; break;
  case 31: foreColor = (!foreBold) ? VGA_C_RED : VGA_C_LIGHTRED; break;
  case 32: foreColor = (!foreBold) ? VGA_C_GREEN : VGA_C_LIGHTGREEN; break;
  case 33: foreColor = (!foreBold) ? VGA_C_BROWN : VGA_C_LIGHTBROWN; break;
  case 34: foreColor = (!foreBold) ? VGA_C_BLUE : VGA_C_LIGHTBLUE; break;
  case 36: foreColor = (!foreBold) ? VGA_C_MAGENTA : VGA_C_LIGHTMAGENTA; break;
  case 35: foreColor = (!foreBold) ? VGA_C_CYAN : VGA_C_LIGHTCYAN; break;
  case 37: foreColor = (!foreBold) ? VGA_C_LIGHTGRAY : VGA_C_WHITE; break;
  case 39: foreColor = defaultForeColor; break; /* Reset to default */

  case 40: backColor = VGA_C_BLACK; break;
  case 41: backColor = VGA_C_RED; break;
  case 42: backColor = VGA_C_GREEN; break;
  case 43: backColor = VGA_C_BROWN; break;
  case 44: backColor = VGA_C_BLUE; break;
  case 45: backColor = VGA_C_MAGENTA; break;
  case 46: backColor = VGA_C_CYAN; break;
  case 47: backColor = VGA_C_LIGHTGRAY; break;
  case 49: backColor = defaultBackColor; break; /* Reset to default */

  default: break;
  }
}

static void flush_escape_buf() {
  int acc = 0;
  for (int i = 0; i < escape_buf_idx; ++i) {
    acc *= 10;
    acc += escape_buf[i] - '0';
  }
  escape_nums[escape_num_idx++] = acc;
  escape_buf_idx = 0;
}

static int handle_escape(char c) {
  switch (c) {
  case '[':
    return 1;
  case '0': case '1': case '2': case '3':
  case '4': case '5': case '6': case '7':
  case '8': case '9':
    escape_buf[escape_buf_idx++] = c;
    return 1;
  case ';':
    flush_escape_buf();
    return 1;

  case 'm':
    flush_escape_buf();
    for (int i = 0; i < escape_num_idx; ++i)
      handle_colour_escape(escape_nums[i]);

    // Fall through

  default:
    escape_buf_idx = 0;
    escape_num_idx = 0;
    // No longer in an escape
    return 0;
  }
}

static void scroll()
{
    // Yes this isn't a byte, but the real info is only in 8 bits.
    uint16_t attributeByte = (backColor<<12) | (foreColor<<8);
    uint16_t blank = 0x20 | attributeByte;
    uint16_t max_rows = vga_get_max_rows();
    if(cursorY >= max_rows) {
        int i;
        for(i = 0; i < ((max_rows-1)*80); i++) {
            VGAPointer[i] = VGAPointer[i+80];
        }
        for(i = ((max_rows-1)*80); i < (max_rows*80); i++) {
            VGAPointer[i] = blank;
        }
        cursorY = (max_rows - 1);
    }
}

static void put_char(char c)
{
    uint16_t attributeByte = (backColor<<12) | (foreColor<<8);
    uint16_t *location;
    if (in_escape) {
        in_escape = handle_escape(c);
        return;
    }
    if (c == 0x08 && cursorX) {
        cursorX--;              // It's backspace
    } else if (c == '\t') {
        cursorX = (cursorX+8) & ~(8-1); // Tab handling
    } else if (c == '\r') {
        cursorX = 0;            // Carraige return
    } else if (c == '\n') {
        cursorX = 0;            // New Line
        cursorY++;
    } else if (c == '\033') {   // VT220 Escape handling
        in_escape = 1;
        return;                 // We're in escape sequence
    } else if (c == '\f') {
        cls();                  // Form Feed
    }else if(c >= ' ') {
        location = VGAPointer + (cursorY*80 + cursorX);
        *location = c | attributeByte;
        cursorX++;
    }
    if (cursorX >= 80){
        cursorX = 0;
        cursorY++;
    }
    scroll();
    vga_move_cursor(cursorX, cursorY);
}

static void cls()
{
    int i;
    for(i=0; i<= (vga_get_max_rows()*80); i++) {
        put_char(' ');
    }
    cursorY = 0;
    cursorX = 0;
    vga_move_cursor(cursorX, cursorY);
}

static int write(console_t *obj, const char *buf, int len)
{
    for (int i = 0; i<len; i++){
        put_char(buf[i]);
    }
    return len;
}

console_t c = {
    .open = NULL,
    .close = NULL,
    .read = NULL,
    .write = &write,
    .flush = NULL,
    .data  = NULL
};

static int register_screen()
{
    VGAPointer = (uint16_t*)vga_get_framebuffer_base();
    vga_set_font(8);
    cls();
    register_console(&c);
    return 0;
}

static prereq_t prereqs[] = { {"console",NULL}, {NULL,NULL} };
MODULE = {
    .name = "x86/VGA/Text",
    .required = prereqs,
    .load_after = NULL,
    .init = &register_screen,
    .fini = NULL
};
