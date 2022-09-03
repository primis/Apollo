#include <sys/hal.h>

// GPIO

enum {
    PERIPHERAL_BASE = 0xFE000000,
    GPFSEL0         = PERIPHERAL_BASE + 0x200000,
    GPSET0          = PERIPHERAL_BASE + 0x20001C,
    GPCLR0          = PERIPHERAL_BASE + 0x200028,
    GPPUPPDN0       = PERIPHERAL_BASE + 0x2000E4
};

enum {
    GPIO_MAX_PIN       = 53,
    GPIO_FUNCTION_ALT5 = 2,
};

enum {
    Pull_None = 0,
};

static void mmio_write(long reg, unsigned int val) { *(volatile unsigned int *)reg = val; }
static unsigned int mmio_read(long reg) { return *(volatile unsigned int *)reg; }

static unsigned int gpio_call(unsigned int pin_number, unsigned int value, unsigned int base, unsigned int field_size, unsigned int field_max) {
    unsigned int field_mask = (1 << field_size) - 1;
  
    if (pin_number > field_max) return 0;
    if (value > field_mask) return 0; 

    unsigned int num_fields = 32 / field_size;
    unsigned int reg = base + ((pin_number / num_fields) * 4);
    unsigned int shift = (pin_number % num_fields) * field_size;

    unsigned int curval = mmio_read(reg);
    curval &= ~(field_mask << shift);
    curval |= value << shift;
    mmio_write(reg, curval);

    return 1;
}

static unsigned int gpio_set     (unsigned int pin_number, unsigned int value) { return gpio_call(pin_number, value, GPSET0, 1, GPIO_MAX_PIN); }
static unsigned int gpio_clear   (unsigned int pin_number, unsigned int value) { return gpio_call(pin_number, value, GPCLR0, 1, GPIO_MAX_PIN); }
static unsigned int gpio_pull    (unsigned int pin_number, unsigned int value) { return gpio_call(pin_number, value, GPPUPPDN0, 2, GPIO_MAX_PIN); }
static unsigned int gpio_function(unsigned int pin_number, unsigned int value) { return gpio_call(pin_number, value, GPFSEL0, 3, GPIO_MAX_PIN); }

static void gpio_useAsAlt5(unsigned int pin_number) {
    gpio_pull(pin_number, Pull_None);
    gpio_function(pin_number, GPIO_FUNCTION_ALT5);
}

// UART

#define AUX_BASE        (PERIPHERAL_BASE + 0x215000)
#define AUX_ENABLES     0x04
#define AUX_MU_IO_REG   0x40
#define AUX_MU_IER_REG  0x44
#define AUX_MU_IIR_REG  0x48
#define AUX_MU_LCR_REG  0x4C
#define AUX_MU_MCR_REG  0x50
#define AUX_MU_LSR_REG  0x54
#define AUX_MU_CNTL_REG 0x60
#define AUX_MU_BAUD_REG 0x68
#define AUX_UART_CLOCK  (500000000)
#define UART_MAX_QUEUE  (16 * 1024)

#define AUX_MU_BAUD(baud) ((AUX_UART_CLOCK/(baud*8))-1)

static unsigned int uart_isWriteByteReady() 
{ 
    return mmio_read(AUX_BASE + AUX_MU_LSR_REG) & 0x20; 
}

static void uart_writeByteBlockingActual(unsigned char ch) 
{
    while (!uart_isWriteByteReady()); 
    mmio_write(AUX_BASE + AUX_MU_IO_REG, (unsigned int)ch);
}

static void rpi_serial_write(const char *buffer) 
{
    while (*buffer) {
        if (*buffer == '\n') {
            uart_writeByteBlockingActual('\r');
        }
        uart_writeByteBlockingActual(*buffer++);
    }
}

static int uart_init() 
{
    mmio_write(AUX_BASE + AUX_ENABLES, 1); //enable UART1
    mmio_write(AUX_BASE + AUX_MU_IER_REG, 0);
    mmio_write(AUX_BASE + AUX_MU_CNTL_REG, 0);
    mmio_write(AUX_BASE + AUX_MU_LCR_REG, 3); //8 bits
    mmio_write(AUX_BASE + AUX_MU_MCR_REG, 0);
    mmio_write(AUX_BASE + AUX_MU_IER_REG, 0);
    mmio_write(AUX_BASE + AUX_MU_IIR_REG, 0xC6); //disable interrupts
    mmio_write(AUX_BASE + AUX_MU_BAUD_REG, AUX_MU_BAUD(115200));
    gpio_useAsAlt5(14);
    gpio_useAsAlt5(15);
    mmio_write(AUX_BASE + AUX_MU_CNTL_REG, 3); //enable RX/TX
    return 0;
}

static int write(console_t *obj, const char *buffer, int len) 
{
    rpi_serial_write(buffer);
    return len;
}

static int open(console_t *obj)
{
    rpi_serial_write("\n\n\n\n");
    return 0;
}

console_t c = {
  .open = &open,
  .close = NULL,
  .read = NULL,
  .write = &write,
  .flush = NULL,
  .data = NULL
};

static int mod_init() 
{
    uart_init();

    register_console(&c);
    return 0;
}

static prereq_t prereqs[] = { {"console",NULL}, {NULL,NULL} };
MODULE = {
  .name = "rpi4/console",
  .required = prereqs,
  .load_after = NULL,
  .init = &mod_init,
  .fini = NULL
};

