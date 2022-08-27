/*
 * (c) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * console.c - x86 console initialization.
 * This utilizes both the vga and vga_console drivers.
 */

#include <sys/hal.h>
#include <sys/resource.h>
#include <sys/device.h>

static resource_t vga_resource = {
  .name = "vga",
  .flags = RESOURCE_META,
  .start = 0xA0000,
  .end   = 0xBFFFF
};

static resource_t framebuffer = {
  .name  = "framebuffer",
  .flags = RESOURCE_MEM
};

static resource_t attribute_control = {
  .name  = "attribute_control",
  .start = 0x3C0,
  .end   = 0x3C1,
  .flags = RESOURCE_IO | RESOURCE_IO_INDEXED | RESOURCE_WIDTH_8
};

static resource_t sequencer = {
  .name  = "sequencer",
  .start = 0x3C4,
  .end   = 0x3C5,
  .flags = RESOURCE_IO | RESOURCE_IO_INDEXED | RESOURCE_WIDTH_8
};

static resource_t graphics = {
  .name  = "graphics",
  .start = 0x3CE,
  .end   = 0x3CF,
  .flags = RESOURCE_IO | RESOURCE_IO_INDEXED | RESOURCE_WIDTH_8
};

static resource_t crtc = {
  .name  = "crtc",
  .start = 0x3D4,
  .end   = 0x3D5,
  .flags = RESOURCE_IO | RESOURCE_IO_INDEXED | RESOURCE_WIDTH_8
};

// Bios video mode 3h, 8 tall font, 80 lines of text.
static uint32_t vga_config[] = {0x3, 8, 80};

static device_t vga_device = {
    .name = "vga",
    .data = &vga_resource
};

static device_t vga_console_device = {
    .name = "vga_console",
    .data = &framebuffer
};

static int console_init()
{
    int ret;
    resource_register(&vga_resource, NULL);
    resource_register(&framebuffer, &vga_resource);
    resource_register(&attribute_control, &vga_resource);
    resource_register(&sequencer, &vga_resource);
    resource_register(&graphics, &vga_resource);
    resource_register(&crtc, &vga_resource);
    ret  = device_register(&vga_device, base_device(), "display/vga");
    ret |= device_register(&vga_console_device, &vga_device, "console/vga");
    ret |= device_init(&vga_device, &vga_config);
    ret |= device_init(&vga_console_device, &vga_config[2]);
    
    return ret;
}

static prereq_t prereqs[] = { {"console",NULL}, {NULL,NULL} };
MODULE = {
    .name = "x86/VGA",
    .required = prereqs,
    .load_after = NULL,
    .init = &console_init,
    .fini = NULL
};