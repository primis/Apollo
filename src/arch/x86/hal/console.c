/*
 * (c) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * console.c - x86 console initialization.
 * Attempts to bring up VGA and Serial Drivers for console
 * output
 */

#include <sys/hal.h>
#include <sys/resource.h>
#include <sys/device.h>

// x86 VGA 
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

static device_t vga_device = {
    .name = "vga",
    .access = &vga_resource
};

static device_t vga_console_device = {
    .name = "vga_console",
    .access = &framebuffer
};

// x86 COM1
static resource_t com1 = {
  .name  = "COM1",
  .start = 0x3F8,
  .end   = 0x3FF,
  .flags = RESOURCE_IO | RESOURCE_WIDTH_8
};

static device_t serial_console_device = {
    .name = "serial_console",
    .access = &com1
};

static int console_init()
{
    int vgaRet, serialRet;
    // Serial Baudrate
    uint32_t baudrate = 115200;
    // Bios video mode 3h, 8 tall font, 80 lines of text.
    uint32_t vga_config[] = {0x3, 8, 80};

    // VGA Console First
    resource_register(&vga_resource, NULL);
    resource_register(&framebuffer, &vga_resource);
    resource_register(&attribute_control, &vga_resource);
    resource_register(&sequencer, &vga_resource);
    resource_register(&graphics, &vga_resource);
    resource_register(&crtc, &vga_resource);
    vgaRet  = device_register(&vga_device, base_device(), "display/vga");
    vgaRet |= device_register(&vga_console_device, &vga_device, "console/vga");
    vgaRet |= device_init(&vga_device, &vga_config);
    vgaRet |= device_init(&vga_console_device, &vga_config[2]);

    // Serial Console Next
    resource_register(&com1, NULL);
    serialRet = device_register(&serial_console_device, base_device(), 
        "console/serial/16550");
    serialRet |= device_init(&serial_console_device, &baudrate);
    
    // If either console succeeds, we succeed
    return (vgaRet && serialRet);
}

static prereq_t prereqs[] = { {"console",NULL}, {NULL,NULL} };
MODULE = {
    .name = "x86/console",
    .required = prereqs,
    .load_after = NULL,
    .init = &console_init,
    .fini = NULL
};
