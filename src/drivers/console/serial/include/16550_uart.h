#ifndef __1655_UART_H
#define __1655_UART_H

#include <adt/ringbuf.h>
#include <arch/x86/ports.h>

// TODO: seperate UART and x86 specific stuff here...
#define UART_BASE_COM1 0x3f8
#define UART_BASE_COM2 0x2f8
#define UART_BASE_COM3 0x3e8
#define UART_BASE_COM4 0x2e8

#define UART_IRQ_COM1 IRQ(4)
#define UART_IRQ_COM2 IRQ(3)
#define UART_IRQ_COM3 IRQ(4)
#define UART_IRQ_COM4 IRQ(3)

// UART REGISTERS

#define UART_RXTX    0    // THR/RBR when DLAB = 0, DLL when DLAB = 1
#define UART_INTEN   1    // IER when DLAB = 0, DLH when DLAB = 1
#define UART_IIFIFO  2    // IIR as Read, FCR for Write
#define UART_LCR     3    // Line Control Register
#define UART_MCR     4    // Modem Control Register
#define UART_LSR     5    // Line Status Register
#define UART_MSR     6    // Modem Status Register
#define UART_SR      7    // Scratch Register

// Interrupt Enable Register (IER)

#define UART_RECV_DATA_INT    0x01 // Enable Recieved Data Available Int
#define UART_TRANS_EMPTY_INT  0x02 // Enable Transmit Register Empty Int
#define UART_RECV_STATUS_INT  0x04 // Enable Receiver Line Status Int
#define UART_MODEM_STATUS_INT 0x08 // Enable Modem Status Int
#define UART_SLEEP_MODE       0x10 // Enable Sleep Mode (16750)
#define UART_LO_POWER_MODE    0x20 // Enable Low Power Mode (16750)
// Bit 7 & 8 are reserved, and are not defined

// Interrupt Identification Register (IIR)
#define UART_INT_PENDING      0x01
#define UART_MODEM_STATUS     0x00
#define UART_TRANSMIT_EMPTY   0x02
#define UART_RECV_DATA_AVAIL  0x04
#define UART_RECVR_LINE_STAT  0x06
#define UART_TIME_OUT_INT     0x0C
#define UART_FIFO_64          0x20
#define UART_NO_FIFO          0x00
#define UART_FIFO_ENABLED_ERR 0x80
#define UART_FIFO_ENABLED     0xC0

// FIFO Control Register (FCR)

#define UART_ENABLE_FIFO      0x01
#define UART_CLEAR_RECV_FIFO  0x02
#define UART_CLEAR_TRANS_FIFO 0x04
#define UART_DMA_MODE_SELECT  0x08
#define UART_ENABLE_FIFO_64   0x20
// FIFO Interrupt Trigger levels for 16 byte FIFO
#define UART_FIFO_INT_1       0x00
#define UART_FIFO_INT_4       0x40
#define UART_FIFO_INT_8       0x80
#define UART_FIFO_INT_14      0xC0
// FIFO Interrupt Trigger levels for 64 byte FIFO
// Yes, these are the same values as above
#define UART_FIFO_INT_16      0x40
#define UART_FIFO_INT_32      0x80
#define UART_FIFO_INT_56      0xC0


// Line Control Register (LCR)i


#define UART_DATA_BIT_5   0x0
#define UART_DATA_BIT_6   0x1
#define UART_DATA_BIT_7   0x2
#define UART_DATA_BIT_8   0x3
#define UART_ONE_STOP_BIT 0x0
#define UART_TWO_STOP_BIT 0x4    // Bit 3
                                 // Bit 3-5
#define UART_NO_PARITY    0x00 // 000
#define UART_ODD_PARITY   0x08 // 001
#define UART_EVEN_PARITY  0x18 // 011
#define UART_MARK_PARITY  0x28 // 101
#define UART_SPACE_PARITY 0x38 // 111
#define UART_BREAK_ENABLE 0x40 // Bit 6
#define UART_DLAB         0x80 // Bit 7


// Modem Control Register (MCR)

#define UART_DATA_TERM_READY  0x01 // Data Terminal Ready
#define UART_REQ_TO_SEND      0x02 // Request to Send
#define UART_AUX_OUTPUT_1     0x04 // Auxiliary Out 1
#define UART_AUX_OUTPUT_2     0x08 // Auxiliary Out 2
#define UART_LOOPBACK_MODE    0x10 // Loopback Mode
#define UART_AUTOFLOW         0x20 // Autoflow Control (16750)


// Line Status Register (LSR)

#define UART_DATA_READY   0x01    // Data Reay
#define UART_OVERRUN_ERR  0x02    // Overrun Error
#define UART_PARITY_ERR   0x04    // Parity Error
#define UART_FRAMING_ERR  0x08    // Framing Error
#define UART_BREAK_INT    0x10    // Break Interrupt
#define UART_EMPTY_TRANS  0x20    // Empty Transmitter Holding Register
#define UART_EMPTY_DATA   0x40    // Empty Data Holding Register
#define UART_FIFO_ERR     0x80    // Error in Recieved FIFO


// Modem Status Register (MSR)

#define UART_DEL_CLR_TO_SEND  0x01    // Delta Clear to Send
#define UART_DEL_DATA_SET_RDY 0x02    // Deltat Data Set Ready
#define UART_TRAIL_RING_IND   0x04    // Trailing Ring Indicator
#define UART_DEL_DATA_CARRIER 0x08    // Delta Data Carrier Detect
#define UART_CLEAR_TO_SEND    0x10    // Clear to Send
#define UART_DATA_SET_READY   0x20    // Data Set Ready
#define UART_RING_IND         0x40    // Ring Indicator
#define UART_CARRIER_DETECT   0x80    // Carrier Detect

// Definitions for types of UART detected
// Should this be an enum?
#define UART_8250    1
#define UART_16450   2
#define UART_16550   3
#define UART_16550A  4
#define UART_16750   5


typedef struct serial_state {
    int base;
    char_ringbuf_t buf;
    int baud;
    int data_bits;
    int parity;
    int stop_bits;
} serial_state_t;



#endif
