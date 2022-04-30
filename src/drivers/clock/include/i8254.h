/*
 * (C) 2022 Apollo Project Developers
 * i8254.h - Definitions for controlling the intel 8253/8254 
 */

#ifndef __ARCH_X86_PIT_H
#define __ARCH_X86_PIT_H
#include <stdint.h>

#define i8254_BASE_FREQ 1193182 /* 1.193181666666666 MHz */

#define i8254_DATA_PORT 0x40

#define i8254_DATA_0    0
#define i8254_DATA_1    1
#define i8254_DATA_2    2
#define i8254_CMD       3

#define i8254_BIN_MODE  0x00    /* 16 Bit Binary Counting Mode */
#define i8254_BCD_MODE  0x01    /* Four Digit Binary Coded Decimal Counting */

#define i8254_MODE_0    0x00    /* Interrupt on Terminal Count */
#define i8254_MODE_1    0x02    /* Hardware re-triggerable One-Shot */
#define i8254_MODE_2    0x04    /* Rate Generator */
#define i8254_MODE_3    0x06    /* Square Wave Generator */
#define i8254_MODE_4    0x08    /* Software Triggered Strobe */
#define i8254_MODE_5    0x0A    /* Hardware Triggered Strobe */

#define i8254_LATCH     0x00    /* Latch Count Value Command */
#define i8254_LOBYTE    0x10    /* Access Low Byte Only */
#define i8254_HIBYTE    0x20    /* Access High Byte Only */
#define i8254_LHBYTE    0x30    /* Access Both Bytes */

#define i8254_CHANNEL_0 0x00    /* Select Channel 0 */
#define i8254_CHANNEL_1 0x40    /* Select Channel 1 */
#define i8254_CHANNEL_2 0x80    /* Select Channel 2 */
#define i8254_READBACK  0xC0    /* 8254 Readback Command */

#endif

