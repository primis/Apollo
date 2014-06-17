/*
** 2014 Apollo Project
** Author: Primis
** <sys/x86/idt.h>
*/
#ifndef _SYS_X86_IDT_H
#define _SYS_X86_IDT_H

#include <sys/interrupt.h>

struct idt_entry_struct
{
	unsigned short 	base_low;	// The Low 16 bits
	unsigned short 	selector;	// Segment Selector
	unsigned char	always0;	// Always Zero Bytes
	unsigned char	flags;		// Flags.
	unsigned short	base_high; 	// Upper 16 bits of address
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

struct idt_ptr_struct
{
	unsigned short	limit;		// How big is it?
	unsigned int	base;		// Base entry
} __attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

typedef struct registers
{
	unsigned int ds;									// DSS
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;// Pusha
	unsigned int int_no, err_code;						// From our Stub
	unsigned int eip, cs, eflags, useresp, ss;			// Pushed by int
} registers_t;

/**********************************************************************/
extern void isr0();		// Differential Pair of interrupts. We use these
extern void isr1();		// To derive the offset of all the other ones.

#endif
