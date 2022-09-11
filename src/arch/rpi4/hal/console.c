/*
 * (c) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * console.c - rpi4 console initialization.
 * Attempts to bring up Serial Driver for console output
 */

#include <sys/hal.h>
#include <sys/resource.h>
#include <sys/device.h>

#define UART_BAUD 115200
#define AUX_ENABLES     0x01
#define AUX_UART_CLOCK  (500000000)
#define AUX_MU_BAUD(baud) ((AUX_UART_CLOCK/(baud*8))-1)

// RPI4 GPIO
static resource_t gpio = {
    .name  = "gpio",
    .start = 0xFE200000,
    .end   = 0xFE2000FF,
    .flags = RESOURCE_MEM | RESOURCE_WIDTH_32
};

// AUX Peripheral Base
static resource_t aux_peripheral = {
  .name  = "aux_peripheral",
  .start = 0xFE215000,
  .end   = 0xFE2150FF,
  .flags = RESOURCE_MEM | RESOURCE_WIDTH_32
};

// RPI4 Mini UART
static resource_t mini_uart = {
  .name  = "mini_uart",
  .start = 0xFE215040,
  .end   = 0xFE21506F,
  .flags = RESOURCE_MEM | RESOURCE_WIDTH_32
};

static device_t serial_console_device = {
    .name = "serial_console",
    .access = &mini_uart
};

static device_t gpio_device = {
    .name = "gpio",
    .access = &gpio
};

static int pincfg(uint32_t pin, uint32_t function)
{
    uint32_t cfg[2];
    cfg[0] = function;
    cfg[1] = pin;
    return device_api_call(&gpio_device, &cfg);
}

static int setup_mini_uart()
{
    int ret;
    uint32_t baudrate = (AUX_MU_BAUD(UART_BAUD) * UART_BAUD);
    uint32_t cmd = 1;

    // Enable the Mini UART
    ret = resource_write(&cmd, &aux_peripheral, (AUX_ENABLES), 1);

    // Set up RX/TX Pins
    ret |= device_register(&gpio_device, base_device(), "gpio/bcm2711");
    ret |= pincfg(14, 7);  // Set Pin 14 to Alternate mode 5 (TX)
    ret |= pincfg(14, 8);  // Turn off Pullup/Pulldown resistors
    ret |= pincfg(15, 7);  // Set Pin 14 to Alternate mode 5 (TX)
    ret |= pincfg(15, 8);  // Turn off Pullup/Pulldown resistors

    // Set up mini UART Proper
    ret |= device_register(&serial_console_device, base_device(), 
        "console/serial/16550");
    ret |= device_init(&serial_console_device, &baudrate);
    return ret;
}

static int mod_init() 
{
    return setup_mini_uart();
}

static prereq_t prereqs[] = { {"console",NULL}, {NULL,NULL} };
MODULE = {
  .name = "rpi4/console",
  .required = prereqs,
  .load_after = NULL,
  .init = &mod_init,
  .fini = NULL
};
