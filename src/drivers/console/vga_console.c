/*
 * (c) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * vga_console.c - Early Kernel mode console for debugging
 * We use VGA cause it's easy, though this can be anything
 * that can output a charecter set
 */

#include <sys/device.h>
#include <sys/hal.h>
#include <sys/resource.h>
#include <errno.h>
#include <string.h>

#include "include/vga.h"

static vga_console_state_t vga_console_state;

static void cls(vga_console_state_t *state);
static void put_char(vga_console_state_t *state, char c);

static void move_cursor(vga_console_state_t *state) {
    uint32_t api_args[3];
    api_args[0] = 0;
    api_args[1] = state->cursor_x;
    api_args[2] = state->cursor_y;

    device_api_call(state->dev->parent, &api_args);
}

static void handle_colour_escape(vga_console_state_t *state, int e) {
  switch (e) {
  case 0:
    /* Reset */
    state->fg_color = DEFAULT_FG_COLOR;
    state->bg_color = DEFAULT_BG_COLOR;
    state->bold = 0;
    break;
  case 1:
    /* Bold */
    if (!state->bold)
      state->fg_color += 8;
    state->bold = 1;
    break;

  case 30: state->fg_color = (!state->bold)?
        VGA_C_BLACK : VGA_C_DARKGRAY; break;
  case 31: state->fg_color = (!state->bold)?
        VGA_C_RED : VGA_C_LIGHTRED; break;
  case 32: state->fg_color = (!state->bold)?
        VGA_C_GREEN : VGA_C_LIGHTGREEN; break;
  case 33: state->fg_color = (!state->bold)?
        VGA_C_BROWN : VGA_C_LIGHTBROWN; break;
  case 34: state->fg_color = (!state->bold)?
        VGA_C_BLUE : VGA_C_LIGHTBLUE; break;
  case 36: state->fg_color = (!state->bold)?
        VGA_C_MAGENTA : VGA_C_LIGHTMAGENTA; break;
  case 35: state->fg_color = (!state->bold)?
        VGA_C_CYAN : VGA_C_LIGHTCYAN; break;
  case 37: state->fg_color = (!state->bold)?
        VGA_C_LIGHTGRAY : VGA_C_WHITE; break;
  case 39: state->fg_color = DEFAULT_FG_COLOR; break; /* Reset to default */

  case 40: state->bg_color = VGA_C_BLACK; break;
  case 41: state->bg_color = VGA_C_RED; break;
  case 42: state->bg_color = VGA_C_GREEN; break;
  case 43: state->bg_color = VGA_C_BROWN; break;
  case 44: state->bg_color = VGA_C_BLUE; break;
  case 45: state->bg_color = VGA_C_MAGENTA; break;
  case 46: state->bg_color = VGA_C_CYAN; break;
  case 47: state->bg_color = VGA_C_LIGHTGRAY; break;
  case 49: state->bg_color = DEFAULT_BG_COLOR; break; /* Reset to default */

  default: break;
  }
}

static void flush_escape_buf(vga_console_state_t *state) {
  int acc = 0;
  for (int i = 0; i < state->escape_buf_idx; ++i) {
    acc *= 10;
    acc += state->escape_buf[i] - '0';
  }
  state->escape_nums[state->escape_num_idx++] = acc;
  state->escape_buf_idx = 0;
}

static int handle_escape(vga_console_state_t *state, char c) {
    switch (c) {
    case '[':
        return 1;
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7':
    case '8': case '9':
        state->escape_buf[state->escape_buf_idx++] = c;
        return 1;
    case ';':
        flush_escape_buf(state);
        return 1;

    case 'm':
        flush_escape_buf(state);
        for (int i = 0; i < state->escape_num_idx; ++i) {
            handle_colour_escape(state, state->escape_nums[i]);
        }

    // Fall through

    default:
        state->escape_buf_idx = 0;
        state->escape_num_idx = 0;
        // No longer in an escape
        return 0;
    }
}

static void scroll(vga_console_state_t *state)
{
    uint16_t *VGAPointer = (uint16_t *)state->framebuffer->start;

    // Yes this isn't a byte, but the real info is only in 8 bits.
    uint16_t attributeByte = (state->bg_color << 12) | (state->fg_color << 8);
    uint16_t blank = 0x20 | attributeByte;

    if(state->cursor_y >= state->max_rows) {
        int i;
        for(i = 0; i < ((state->max_rows-1)*state->max_cols); i++) {
            VGAPointer[i] = VGAPointer[i+state->max_cols];
        }
        for(i = ((state->max_rows-1)*80); i < (state->max_rows*80); i++) {
            VGAPointer[i] = blank;
        }
        state->cursor_y = (state->max_rows - 1);
    }
}

static void put_char(vga_console_state_t *state, char c)
{
    uint16_t attributeByte = (state->bg_color << 12) | (state->fg_color << 8);
    uint16_t *VGAPointer = (uint16_t *)state->framebuffer->start;
    uint16_t *location;

    if (state->in_escape) {
        state->in_escape = handle_escape(state, c);
        return;
    }
    if (c == 0x08 && state->cursor_x) {
        state->cursor_x--;                              // Backspace
    } else if (c == '\t') {
        state->cursor_x= (state->cursor_x + 8) & ~(8-1);// Tab
    } else if (c == '\r') {
        state->cursor_x = 0;                            // Carraige return
    } else if (c == '\n') {
        state->cursor_x = 0;                            // New Line
        state->cursor_y++;
    } else if (c == '\033') {                           // VT220 Escape
        state->in_escape = 1;
        return;                                     // We're in escape sequence
    } else if (c == '\f') {
        cls(state);                                     // Form Feed
    }else if(c >= ' ') {
        location = VGAPointer +
            (state->cursor_y * state->max_cols + state->cursor_x);
        *location = c | attributeByte;
        state->cursor_x++;
    }
    if (state->cursor_x >= state->max_cols){
        state->cursor_x = 0;
        state->cursor_y++;
    }
    scroll(state);
}

static void cls(vga_console_state_t *state)
{
    int i;
    for(i=0; i <= (state->max_rows * state->max_cols); i++) {
        put_char(state, ' ');
    }
    state->cursor_y = 0;
    state->cursor_x = 0;
}

static int write(console_t *obj, const char *buf, int len)
{
    vga_console_state_t *state = (vga_console_state_t *)obj->data;

    for (int i = 0; i < len; i++){
        put_char(state, buf[i]);
    }
    move_cursor(state);
    return len;
}

static int open(console_t *obj)
{
    vga_console_state_t *state = (vga_console_state_t *)obj->data;
    cls(state);
    return 0;
}

console_t c = {
    .open = &open,
    .close = NULL,
    .read = NULL,
    .write = &write,
    .flush = NULL,
    .data  = NULL
};

static int init(device_t *d, void *p)
{
    uint32_t *config = (uint32_t *)p;
    c.data = &vga_console_state;        // Make this a malloc at some point

    vga_console_state.framebuffer       = (resource_t *)d->access;
    vga_console_state.cursor_x          = 0;
    vga_console_state.cursor_y          = 0;
    vga_console_state.in_escape         = 0;
    vga_console_state.escape_buf_idx    = 0;
    vga_console_state.escape_num_idx    = 0;
    vga_console_state.fg_color          = DEFAULT_FG_COLOR;
    vga_console_state.bg_color          = DEFAULT_BG_COLOR;
    vga_console_state.bold              = 0;
    vga_console_state.max_rows          = config[0];
    vga_console_state.max_cols          = config[1];
    vga_console_state.dev               = d;
    cls(&vga_console_state);
    register_console(&c);
    return 0;
}

DRIVER = {
  .compat = "console/vga",
  .init = &init
};
