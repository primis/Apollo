/*
 * (c) 2018 Apollo Developers
 * Serial.c - x86 Serial Driver
 */


// TODO: Implement interrupts for read / write so we dont end up with busy loops


#include <arch/x86/ports.h>
#include <adt/ringbuf.h>
#include <arch/x86/serial.h>
#include <sys/hal.h>

#define UART_BUFSZ 32


static console_t consoles[4];
static serial_state_t states[4];
static char bufs[4][UART_BUFSZ];
static int bases[4] =  {UART_BASE_COM1, UART_BASE_COM2,
                        UART_BASE_COM3, UART_BASE_COM4};

static void send_data(int base, uint8_t byte)
{
    // Busy loop (See TODO on top)
    while((read_register(base, UART_LSR) & UART_EMPTY_TRANS) == 0);

    write_register(base, UART_RXTX, byte);
}

static int write(console_t *obj, const char *buf, int len)
{
    int i;
    serial_state_t *status = (serial_state_t*)obj->data;

    for (i = 0; i < len; i++) {
        if(buf[i] == '\n') {
            send_data(status->base, '\r'); // Have to send a carraige return!!
        }
        send_data(status->base, buf[i]);
    }
    return len;
}

static int open(console_t *obj)
{
    uint8_t buf = 0;
    serial_state_t *status = (serial_state_t*)obj->data;
    uint8_t base = status->base;

    if(status->baud > 115200) {
        return -1; // Baudrate too high
    }

    // set up some status bits
    buf =  (status->data_bits);
    buf += (status->parity);
    buf += (status->stop_bits);

    write_register(base, UART_INTEN, 0);
    // Enable Divisor Latch Access Bit (DLAB), to set the baud rate
    write_register(base, UART_LCR, UART_DLAB);
    write_register(base, UART_RXTX, (115200 / status->baud) & 0xFF);
    write_register(base, UART_INTEN, (115200 / status->baud) >> 8);
    write_register(base, UART_LCR, buf);
    // TODO: make sure we can actually use the FIFO!!
    // FIFO enable and clear
    buf = (UART_ENABLE_FIFO | UART_CLEAR_RECV_FIFO | UART_CLEAR_TRANS_FIFO);
    // FIFO buffer size
    buf += UART_FIFO_INT_14;
    write_register(base, UART_IIFIFO, buf);
    write_register(base, UART_MCR, 0x0b); // TODO: MAGIC NUMBERS!!!
    write_register(base, UART_INTEN, 0x0C);

    return 0;
}

// Detect if a UART exists, and return a number depending on type
static uint8_t is_connected(int base) {
    uint8_t iir;
    uint8_t msr;

    // First off, check Modem Status, make sure a chip exists at that address
    msr = read_register(base, UART_MSR);
    if(!(msr & (UART_DATA_SET_READY | UART_CLEAR_TO_SEND))) {
        // No serial device.
        return 0;
    }
    if(msr == 0xFF) { // Shows up for a device not present
        return 0;
    }

    // Test the FIFO Flags to determine the chip
    // If you're interested, the flags being set are in the header
    // enumerated: 0x1, 0x2, 0x4, 0x20, 0xC0

    write_register(base, UART_IIFIFO, 0x87);

    // Read back the IIR to see what flags stuck
    iir = read_register(base, UART_IIFIFO);

    // These two together are FIFO's working.
    // There was a bug in the 16550 that made it not show up right.
    // So we test seperately.

    if(iir & 0x40) {
        if(iir & 0x80) {
            if(iir & UART_FIFO_64) {
                return UART_16750; // Everything works!!!
            }
            return UART_16550A;    // FIFO's work!
        }
        return UART_16550;         // Ok so FIFO's work, kinda
    }


    // test scratch register with random value. We're gonna use 0x42
    write_register(base, UART_SR, 0x42);

    // now try to read it back
    if (read_register(base, UART_SR) == 0x42) {
        return UART_16450;
    }
    // Ok, so scratch register isn't showing up.
    // It must be either a 8250, 8250A, or 8250B
    return UART_8250;
}


static int register_serial()
{
    int i;
    // TODO: Scan for bases instead of assuming
    for(i = 0; i < 2; i++) {
        if(!is_connected(bases[i])) {
            continue; // no serial port here, go to next one
        }
        states[i].base      = bases[i];
        states[i].buf       = make_char_ringbuf(bufs[i], UART_BUFSZ);
        states[i].baud      = 115200;
        states[i].parity    = UART_NO_PARITY;
        states[i].data_bits = UART_DATA_BIT_8;
        states[i].stop_bits = UART_ONE_STOP_BIT;
        consoles[i].open    = &open;
        consoles[i].close   = NULL;
        consoles[i].read    = NULL;  // TODO: implement READ
        consoles[i].write   = &write;
        consoles[i].flush   = NULL;
        consoles[i].data    = (void*)&states[i];

        (void)register_console(&consoles[i]);

    }
    return 0;
}

static prereq_t prereqs[] = { {"console",NULL}, {NULL,NULL} };

MODULE = {
    .name       = "x86/serial",
    .required   = prereqs,
    .load_after = NULL,
    .init       = &register_serial,
    .fini       = NULL
};
