
/**
 * @brief Set VGA font.
 * 
 * @param height Either 8 or 16
 */

#include <stdint.h>
#include <arch/x86/ports.h>
#include <string.h>
#include <sys/hal.h>

#include "include/vga.h"

extern uint8_t vga_8x8_font[2048];
extern uint8_t vga_8x16_font[4096];
static uint16_t max_rows = 25;

void* vga_get_framebuffer_base();

static void write_index_reg(uint16_t index, uint8_t offset, uint8_t data)
{
  outb(index, offset);
  outb(index + 1, data);
}

static uint8_t read_index_reg(uint16_t index, uint8_t offset)
{
  outb(index, offset);
  return inb(index + 1);
}

/**
 * @brief Set the VGA plane (0-3) 
 * 
 * @param p plane ID (0-3)
 */
static void set_plane(int p)
{
  uint8_t pmask;
  p &= 3;
  pmask = 1 << p;

  // Set Read Plane
  write_index_reg(VGA_INDEX_2, VGA_READ_MAP_SELECT, p);
  // Set Write Plane
  write_index_reg(VGA_INDEX_1, VGA_MAP_MASK, pmask); 
}

/**
 * @brief Write a font to VGA memory so it can be used on screen
 * Warning: overwrites existing font in memory!
 * @param buf - Buffer to font location
 * @param height - How tall in pixels the font is (can be 1-32)
 */
static void write_font(uint8_t *buf, uint8_t height)
{
  uint8_t map_mask, memory_mode, read_map_sel, graphics_mode, misc_graphics;
  uint8_t new_memory_mode;
  
  void *framebuffer_base = vga_get_framebuffer_base();
  int i;

  // Save State of things we need to modify
  map_mask = read_index_reg(VGA_INDEX_1, VGA_MAP_MASK);
  memory_mode = read_index_reg(VGA_INDEX_1, VGA_SEQ_MEM_MODE);
  read_map_sel = read_index_reg(VGA_INDEX_2, VGA_READ_MAP_SELECT);
  graphics_mode = read_index_reg(VGA_INDEX_2, VGA_GRAPHICS_MODE);
  misc_graphics = read_index_reg(VGA_INDEX_2, VGA_MISC_GRAPHICS);

  new_memory_mode = memory_mode | 0x04;
  new_memory_mode &= ~0x08;

  // Turn off even-odd addressing (set flat)
  write_index_reg(VGA_INDEX_1, VGA_SEQ_MEM_MODE, new_memory_mode);
  write_index_reg(VGA_INDEX_2, VGA_GRAPHICS_MODE, (graphics_mode & ~0x10));
  write_index_reg(VGA_INDEX_2, VGA_MISC_GRAPHICS, (misc_graphics & ~0x02));

  // Set plane to P4
  set_plane(2);
  
  // write Font 0 (Standard)
  for(i = 0; i < 256; i++)
  {
    memcpy(framebuffer_base + (i * 32), buf, height);
    buf += height;
  }

  // Restore State of things modified.
  write_index_reg(VGA_INDEX_1, VGA_MAP_MASK, map_mask);
  write_index_reg(VGA_INDEX_1, VGA_SEQ_MEM_MODE, memory_mode);
  write_index_reg(VGA_INDEX_2, VGA_READ_MAP_SELECT, read_map_sel);
  write_index_reg(VGA_INDEX_2, VGA_GRAPHICS_MODE, graphics_mode);
  write_index_reg(VGA_INDEX_2, VGA_MISC_GRAPHICS, misc_graphics);
}

static void set_8_tall_font()
{
  uint8_t config_data;

  // Set char height to 8 tall  
  config_data = read_index_reg(VGA_INDEX_3, VGA_MAX_SCAN_LINE);
  config_data &= 0xE0;
  config_data += 7;
  write_index_reg(VGA_INDEX_3, VGA_MAX_SCAN_LINE, config_data);
  
  // Change cursor scanline start / end
  write_index_reg(VGA_INDEX_3, VGA_CURSOR_START, 0x06);
  write_index_reg(VGA_INDEX_3, VGA_CURSOR_END, 0x07);

  // Write the 8x8 font to VGA memory
  write_font(&vga_8x8_font[0], 8);
}

static void set_16_tall_font()
{
  uint8_t config_data;

  // Set char height to 8 tall  
  config_data = read_index_reg(VGA_INDEX_3, VGA_MAX_SCAN_LINE);
  config_data &= 0xE0;
  config_data += 15;
  write_index_reg(VGA_INDEX_3, VGA_MAX_SCAN_LINE, config_data);
  
  // Change cursor scanline start / end
  write_index_reg(VGA_INDEX_3, VGA_CURSOR_START, 0x13);
  write_index_reg(VGA_INDEX_3, VGA_CURSOR_END, 0x14);

  // Write the 8x8 font to VGA memory
  write_font(&vga_8x16_font[0], 8);
}

void vga_set_font(int height)
{
    if(height == 8)
    {
        set_8_tall_font();
        max_rows = 50;
        return;
    }
    set_16_tall_font();
    max_rows = 25;
}

uint16_t vga_get_max_rows() {
  return max_rows;
}

void vga_move_cursor(uint16_t cursorX, uint16_t cursorY)
{
    uint16_t cursorLocation = cursorY * 80 + cursorX;
    write_index_reg(VGA_INDEX_3, VGA_CURSOR_HIGH_BYTE, cursorLocation >> 8);
    write_index_reg(VGA_INDEX_3, VGA_CURSOR_LOW_BYTE, cursorLocation);
}

/**
 * @brief Retrieve the base address of the VGA Frambuffer
 * 
 *  Bits 2 and 3 of Misc graphics register are the memory map select
 *  This can be 0xA0000, 0xB0000, or 0xB8000 Depending on the bits
 * @return void* address of the framebuffer's base
 */
void* vga_get_framebuffer_base()
{
  uint8_t map_sel;
  uint32_t framebuffer_base;
  map_sel = read_index_reg(VGA_INDEX_2, VGA_MISC_GRAPHICS);
  // Isolate bits 2 and 3
  map_sel >>= 2;
  map_sel &= 3;

  switch(map_sel)
  {
    case 0:   // 128k window, 0xA0000 - 0xBFFFF
    case 1:   //  64k window, 0xA0000 - 0xAFFFF
      framebuffer_base = VGA_BASE_POINTER_1;
      break;
    case 2:   //  32K window, 0xB0000 - 0xB7FFF
      framebuffer_base = VGA_BASE_POINTER_2;
      break;
    case 3:   //  32K window, 0xB8000 - 0xBFFFF
      framebuffer_base = VGA_BASE_POINTER_3;
      break;
  }
  return (void*)framebuffer_base;
}