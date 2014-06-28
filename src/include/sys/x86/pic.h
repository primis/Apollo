/*
 * (c) 2014 Apollo Developers
 * <sys/x86/pic.h> - Useful functions for masking and unmasking the vectors on
 * The PIC.
 */

#ifndef _SYS_X86_PIC_H
#define _SYS_X86_PIC_H

void setIRQMask(int);   // Forbit the Line from interrupting the CPU
void clearIRQMask(int); // Let the Line interrupt the CPU

#endif
