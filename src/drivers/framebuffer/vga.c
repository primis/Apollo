#include <stdint.h>
#include <string.h>
#include <sys/hal.h>
#include <sys/resource.h>
#include <sys/device.h>
#include <errno.h>

#include "include/vga.h"

extern uint8_t vga_8x8_font[2048];
extern uint8_t vga_8x16_font[4096];

static uint16_t h_res = 640, v_res=400;

static resource_t *vga_base, *framebuffer, *attribute_control, *sequencer;
static resource_t *graphics, *crtc;

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
    resource_write(&p, graphics, VGA_READ_MAP_SELECT, 1);
    // Set Write Plane
    resource_write(&pmask, sequencer, VGA_MAP_MASK, 1);
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
    uint8_t new_memory_mode, new_graphics_mode, new_misc_graphics;

    void *framebuffer_base = (void *)framebuffer->start;
    int i;

    // Save State of things we need to modify
    resource_read(&map_mask, sequencer, VGA_MAP_MASK, 1);
    resource_read(&memory_mode, sequencer, VGA_SEQ_MEM_MODE, 1);
    resource_read(&read_map_sel, graphics, VGA_READ_MAP_SELECT, 1);
    resource_read(&graphics_mode, graphics, VGA_GRAPHICS_MODE, 1);
    resource_read(&misc_graphics, graphics, VGA_MISC_GRAPHICS, 1);

    new_memory_mode    = memory_mode | 0x04;
    new_memory_mode   &= ~0x08;
    new_graphics_mode = graphics_mode & ~0x10;
    new_misc_graphics = misc_graphics & ~0x02;

    // Turn off even-odd addressing (set flat)
    resource_write(&new_memory_mode, sequencer, VGA_SEQ_MEM_MODE, 1);
    resource_write(&new_graphics_mode, graphics, VGA_GRAPHICS_MODE, 1);
    resource_write(&new_misc_graphics, graphics, VGA_MISC_GRAPHICS, 1);

    // Set plane to P4
    set_plane(2);

    // write Font 0 (Standard)
    for(i = 0; i < 256; i++)
    {
        memcpy(framebuffer_base + (i * 32), buf, height);
        buf += height;
    }

    // Restore State of things modified.
    resource_write(&map_mask, sequencer, VGA_MAP_MASK, 1);
    resource_write(&memory_mode, sequencer, VGA_SEQ_MEM_MODE, 1);
    resource_write(&read_map_sel, graphics, VGA_READ_MAP_SELECT, 1);
    resource_write(&graphics_mode, graphics, VGA_GRAPHICS_MODE, 1);
    resource_write(&misc_graphics, graphics, VGA_MISC_GRAPHICS, 1);
}

static void set_text_mode(uint8_t *font, uint8_t height, 
    uint8_t cursor_start, uint8_t cursor_end)
{
    uint8_t config_data;

    // Set char height to 8 tall  
    resource_read(&config_data, crtc, VGA_MAX_SCAN_LINE, 1);
    config_data &= 0xE0;
    config_data += (height - 1);
    resource_write(&config_data, crtc, VGA_MAX_SCAN_LINE, 1);

    // Change cursor scanline start / end
    resource_write(&cursor_start, crtc, VGA_CURSOR_START, 1);
    resource_write(&cursor_end, crtc, VGA_CURSOR_END, 1);

    // Write the font to VGA memory
    write_font(font, height);

    // Enable character generator latches
    resource_read(&config_data, graphics, VGA_MISC_GRAPHICS, 1);
    config_data &= ~0x01; // Clear the Alphanumeric mode Disable flag
    resource_write(&config_data, graphics, VGA_MISC_GRAPHICS, 1);
}

static uint32_t vga_get_framebuffer_base_offset()
{
    uint8_t map_sel;

    resource_read(&map_sel, graphics, VGA_MISC_GRAPHICS, 1);

    // Isolate bits 2 and 3
    map_sel >>= 2;
    map_sel &= 3;

    switch(map_sel)
    {
        case 0:   // 128k window, No Offset
        case 1:   //  64k window, No Offset
            return 0;
        case 2:   //  32K window, 0x10000 Offset
            return 0x10000;
        case 3:   //  32K window, 0x18000 Offset
            return 0x18000;
    }
    return 0;
}

static uint32_t vga_get_framebuffer_size()
{
    uint8_t map_sel;

    resource_read(&map_sel, graphics, VGA_MISC_GRAPHICS, 1);
    // Isolate bits 2 and 3
    map_sel >>= 2;
    map_sel &= 3;

    switch(map_sel)
    {
        case 0:   // 128k window, No Offset
            return 0x20000;
        case 1:   //  64k window, No Offset
            return 0x10000;
        case 2:   //  32K window, 0x10000 Offset
            return 0x8000;
        case 3:   //  32K window, 0x18000 Offset
            return 0x8000;
    }
    return 0;
}

static void vga_move_cursor(uint16_t cursorX, uint16_t cursorY)
{
    uint16_t cursorLocation = cursorY * (h_res / 8) + cursorX;
    uint8_t cursorByte = cursorLocation >> 8;
    resource_write(&cursorByte, crtc, VGA_CURSOR_HIGH_BYTE, 1);
    cursorByte = cursorLocation;
    resource_write(&cursorByte, crtc, VGA_CURSOR_LOW_BYTE, 1);
}

static int api_call(device_t *dev, void *api_data)
{
    uint32_t *data = (uint32_t *)api_data;
    switch(data[0]) 
    {
        case 0: // move cursor (data[1] = cursorX, data[2] = cursorY)
            vga_move_cursor(data[1], data[2]);
            break;
        default:
            return -EOPNOTSUPP;
    }
    return 0;
}

/**
 * @brief Set VGA Video mode (Think BIOS video mode numbers)
 * 
 * @param mode - Video mode as described by BIOS INT 0x10 / AH=0
 * @param font_height - Font height desired (for graphics modes, set to 0)
 * @return int -1 if mode unsupported or error
 */
static int vga_set_video_mode(uint32_t mode, uint32_t font_height)
{
    switch(mode)
    {             // Resolution Colors T/G  Origin
        case 0x00:  // 320x200    16     T    CGA
        case 0x01:  // 320x200    16     T    CGA
            return -EOPNOTSUPP;
        case 0x02:  // 640x400    16     T    CGA
        case 0x03:  // 640x400    16     T    CGA
            h_res = 640;
            v_res = 400;
            break; // Our standard mode! :D
        case 0x04:  // 320x200    4      G    CGA
        case 0x05:  // 320x200    4      G    CGA
        case 0x06:  // 640x200    2      G    CGA
        case 0x07:  // 640x400    mono   T    MDA
        case 0x0D:  // 320x200    16     G    EGA
        case 0x0E:  // 640x200    16     G    EGA
        case 0x0F:  // 640x350    mono   G    EGA
        case 0x10:  // 640x350    16     G    EGA
        case 0x11:  // 640x480    2      G    MCGA
        case 0x12:  // 640x480    16     G    VGA
        case 0x13:  // 320x200    256    G    MCGA
        default:    // Anything else is unsupported by VGA
            return -EOPNOTSUPP;
    }

    // Remap framebuffer location, as it might have moved.
    framebuffer->start += vga_base->start + vga_get_framebuffer_base_offset();
    framebuffer->end = vga_base->end + vga_get_framebuffer_size();

    switch(font_height)
    {
        case 8:
            // use 8x8 vga font, 8 px high, cursor on scanlines 6-7
            set_text_mode(vga_8x8_font, 8, 6, 7);
            break;
      case 16:
            // use 8x16 vga font, 8 px high, cursor on scanlines 14-15
            set_text_mode(vga_8x16_font, 16, 14, 15);
            break;
    }

    return 0;
}

static int init(device_t *dev, void *p)
{
    uint32_t *config = (uint32_t *)p;
        if(config == NULL) {
        return -EINVAL;
    }
    // Save the top level resource locally
    vga_base = (resource_t *)dev->data;

    // Fill out all the sub-resources!
    framebuffer = resource_find("framebuffer", vga_base);
    if(framebuffer == NULL) {
        return -1;
    }
    attribute_control = resource_find("attribute_control", vga_base);
    if(attribute_control == NULL) {
        return -1;
    }
    sequencer = resource_find("sequencer", vga_base);
    if(sequencer == NULL) {
        return -1;
    }
    graphics = resource_find("graphics", vga_base);
    if(graphics == NULL) {
        return -1;
    }
    crtc = resource_find("crtc", vga_base);
    if(crtc == NULL) {
        return -1;
    }

    return vga_set_video_mode(config[0], config[1]);
}

DRIVER = {
    .compat = "display/vga",
    .api = &api_call,
    .init = &init
};
