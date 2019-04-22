/*
 * (c) 2018 Apollo Developers
 * For terms, see LICENSE
 * Serial.c - x86 Serial Driver
 */


// TODO: Implement interrupts for read / write so we dont end up with busy loops


#include <arch/x86/ports.h>
#include <adt/ringbuf.h>
#include <arch/x86/serial.h>
#include <sys/hal.h>

#define UART_BUFSZ 32

// We assume a max of 4 COM lines
// TODO: implement using malloc!!!
static console_t consoles[4];
static serial_state_t states[4];
static char bufs[4][UART_BUFSZ];
static int bases[4] = {UART_BASE_COM1, UART_BASE_COM2,
                       UART_BASE_COM3, UART_BASE_COM4};
static int irqs[4]  = {UART_IRQ_COM1, UART_IRQ_COM2,
                       UART_IRQ_COM3, UART_IRQ_COM4};
static void send_data(int base, uint8_t byte)
{
    // Busy loop (See TODO on top)
    while((read_register(base, UART_LSR) & UART_EMPTY_TRANS) == 0);

    write_register(base, UART_RXTX, byte);
}


static uint8_t is_data_ready(int base)
{
    // Check to see if UART has anything in buffer for us
    if (read_register(base, UART_LSR) & UART_DATA_READY) {
        return 1;
    }
    return 0;
}

static uint8_t get_data_nonblock(int base)
{
    return read_register(base, UART_RXTX);
}

static uint8_t get_data_block(int base)
{
    while (!is_data_ready(base));
    return get_data_nonblock(base);
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
    // flush the serial buffer a bit
    read_register(base, UART_INTEN);
    read_register(base, UART_INTEN);
    read_register(base, UART_INTEN);
    read_register(base, UART_INTEN);

    // Disable interrupts from the device
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
    // Enable FIFO, clear 14 byte buffer
    // TODO: is this the right size???
    write_register(base, UART_IIFIFO, buf);
    // Enable IRQ's Ready to send / DSR is turned on
    write_register(base, UART_MCR, 0x0b); // TODO: MAGIC NUMBERS!!!
    // Re-enable serial interrupts (from the uart side)
    write_register(base, UART_INTEN, 0x0C);

    return 0;
}

// Detect if a UART exists, and return a number depending on type
static uint8_t is_connected(int base) {
    uint8_t iir;
    uint8_t msr;
    /**
    // TODO: Detect if we're in QEMU via CPUID or ACPI tables
    // Qemu will hang if we try to detect
    if (base == bases[0]) {
        return 1; // Not sure what QEMU emulates...
    }
    */
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

static int serial_interrupt_handler(struct regs *regs, void *p)
{
    serial_state_t *state = (serial_state_t*)p;
    uint8_t data = get_data_block(state->base);
    char_ringbuf_write(&state->buf, (char*)&data, 1);

    return 0;
}

static int read(console_t *obj, char *buf, int len)
{
    int n;
    uint8_t c;

    if (len < 1) {
        return 0; // why'd you even ask for zero bytes?
    }
    serial_state_t *COM = (serial_state_t*)obj->data;
    n = char_ringbuf_read(&COM->buf, buf, len);
    if (n) {        // Is it a non null?
        return n;   // Hooray! no waiting :D
    }
    while (is_data_ready(COM->base)) {
        c = get_data_nonblock(COM->base);
        if(c == '\r') {
            c = '\n';
        }
        char_ringbuf_write(&COM->buf, (char*)&c, 1);
    }
    return char_ringbuf_read(&COM->buf, buf, len);
}

static int register_serial()
{
    int i;

    // TODO: Scan for bases instead of assuming
    // TODO: don't set like this, check kernel line from multiboot
    // TODO: allow for more than 4 (right now that's limit from hardcoding)
    // TODO: can only use 2 unless we distunguish in IRQ which is which
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
        consoles[i].read    = &read;
        consoles[i].write   = &write;
        consoles[i].flush   = NULL;
        consoles[i].data    = (void*)&states[i];
        register_console(&consoles[i]);
        register_interrupt_handler(irqs[i], &serial_interrupt_handler,
                                         (void*)&states[i]);
    }
    return 0;
}

static prereq_t prereqs[] = {{"console",NULL}, {"interrupts",NULL}, {NULL,NULL}};

MODULE = {
    .name       = "x86/serial",
    .required   = prereqs,
    .load_after = NULL,
    .init       = &register_serial,
    .fini       = NULL
};
