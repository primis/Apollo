/*
 * (c) 2022 Apollo Developers
 * For terms, see LICENSE
 * 16550_uart.c - Standard UART Driver
 */

#include <sys/device.h>
#include <sys/resource.h>
#include <sys/hal.h>
#include <stdint.h>
#include "include/16550_uart.h"

static int is_transmit_empty(device_t *d) {
    uint32_t data;
    resource_read(&data, d->access, UART_LSR, 1);
    if(data & UART_EMPTY_TRANS) {
        return 1;
    }
    return 0;
}

static void write_char(device_t *d, char c)
{
    while(!is_transmit_empty(d));
    uint32_t actual = c;
    resource_write(&actual, d->access, UART_RXTX, 1);
}

static int write(console_t *obj, const char *buffer, int len)
{
    int i;
    device_t *dev = (device_t *)obj->data;
    for (i=0; i<len; i++) {
        if(buffer[i] == '\n')
        {
            write_char(dev, '\r');
        }
        write_char(dev, buffer[i]);
    }
    return len;
}

static void flush(console_t *obj)
{
    device_t *dev = (device_t *)obj->data;
    uint32_t data = 0;
    while((data & UART_EMPTY_DATA) == 0) {
        resource_read(&data, dev->access, UART_LSR, 1);
    }
}

static int open(console_t *obj) 
{
    device_t *dev = (device_t *)obj->data;
    // Enable Data Terminal Ready, and Request to Send
    uint32_t data = UART_DATA_TERM_READY | UART_REQ_TO_SEND | 
        UART_AUX_OUTPUT_2;  // Aux Output 2 enables the IRQ on PC compatibles        
    resource_write(&data, dev->access, UART_MCR, 1);    // Modem Control Register

    data = UART_ENABLE_FIFO | UART_FIFO_INT_8 | UART_CLEAR_RECV_FIFO |
        UART_CLEAR_TRANS_FIFO;  // Clear out the FIFO's, reset their state.
    resource_write(&data, dev->access, UART_IIFIFO, 1); // FIFO Control Register

    data = UART_RECV_DATA_INT | UART_TRANS_EMPTY_INT |
        UART_RECV_STATUS_INT | UART_MODEM_STATUS_INT;  // Interrupt types
    resource_write(&data, dev->access, UART_INTEN, 1);  // Enable Interrupts

    return 0;
}

static int close(console_t *obj) 
{
    device_t *dev = (device_t *)obj->data;

    uint32_t data = 0;
    resource_write(&data, dev->access, UART_MCR, 1);  // Disable Modem Controls
    resource_write(&data, dev->access, UART_INTEN, 1);   // Disable Interrupts

    return 0;
}

static console_t dev_serial = {
    .open = open,
    .close = close,
    .write = write,
    .flush = flush,
};

// We always want 8N1, baudrate is defined by p (as a uint16_t)
static int init(device_t *d, void *p)
{
    // UART Clock is in reference to 115200 baud
    uint16_t baud_div = (*(uint32_t *)p / 115200);
    uint32_t data;

    data = 0;
    resource_write(&data, d->access, UART_INTEN, 1);    // Mask All Interrupts
    
    data = UART_DLAB;
    resource_write(&data, d->access, UART_LCR, 1);      // Enable DLAB
    
    data = (baud_div & 0xFF);
    resource_write(&data, d->access, UART_RXTX, 1);     // Divisor Low Byte
    data = (baud_div >> 8) & 0xFF;
    resource_write(&data, d->access, UART_INTEN, 1);    // Divisor High Byte
    
    data = UART_DATA_BIT_8 | UART_NO_PARITY | UART_ONE_STOP_BIT;    // 8N1
    resource_write(&data, d->access, UART_LCR, 1);      // Line Protocol
    dev_serial.data = d;
    return register_console(&dev_serial);
}

DRIVER = {
  .compat = "console/serial/16550",
  .init = &init
};
