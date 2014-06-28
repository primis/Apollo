/* 
 * (c) 2014 Apollo Developers
 * <sys/x86/pit.h> All Things related to the Programmable Interrupt Timer
 */

#ifndef _SYS_X86_PIT_H
#define _SYS_X86_PIT_H

#include <sys/x86/ports.h>
#include <sys/x86/pic.h>

#define PIT_BASE_FREQ   1193182 /* 1.193181666666666 MHz */

#define PIT_DATA_0      0x40
#define PIT_DATA_1      0x41
#define PIT_DATA_2      0x42
#define PIT_CMD         0x43

#define PIT_BIN_MODE    0x00    /* 16 Bit Binary Counting Mode */
#define PIT_BCD_MODE    0x01    /* Four Digit Binary Coded Decimal Counting */

#define PIT_MODE_0      0x00    /* Interrupt on Terminal Count */
#define PIT_MODE_1      0x02    /* Hardware re-triggerable One-Shot */
#define PIT_MODE_2      0x04    /* Rate Generator */
#define PIT_MODE_3      0x06    /* Square Wave Generator */
#define PIT_MODE_4      0x08    /* Software Triggered Strobe */
#define PIT_MODE_5      0x0A    /* Hardware Triggered Strobe */

#define PIT_LATCH       0x00    /* Latch Count Value Command */
#define PIT_LOBYTE      0x10    /* Access Low Byte Only */
#define PIT_HIBYTE      0x20    /* Access High Byte Only */
#define PIT_LHBYTE      0x30    /* Access Both Bytes */

#define PIT_CHANNEL_0   0x00    /* Select Channel 0 */
#define PIT_CHANNEL_1   0x40    /* Select Channel 1 */
#define PIT_CHANNEL_2   0x80    /* Select Channel 2 */
#define PIT_READBACK    0xC0    /* 8254 Readback Command */

#endif
