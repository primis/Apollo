#include <sys/x86/ports.h>

/***********************************************************************
** Ok, this requires a bit of explanation;
** These are raw hardware calls that require assembly, By choice I use
** intel syntax.
**
** Why inline?
**		These calls are mostly one liners, the extra call and return is
** 		bad overhead that would also get messed up during optimization
**
** Whats that "memory" thing?
**		CLI and STI need not be moved from where they are. Optimization
**		should NEVER touch their location to prevent spinlocks from
**		getting messed up. The "memory" in the third colon tells gcc to
**		leave that code alone completely.
***********************************************************************/


inline void outb(unsigned short _port, unsigned char _data)
{
	asm volatile("outb dx, al"
		:								// Output <emtpy>
		: "d"(_port), "a" (_data)		// Input
	);
}

inline unsigned char inb(unsigned short _port)
{
	unsigned char data;
	asm volatile("inb al, dx"
		: "=a" (data)					// Output
		: "d" (_port)					// Input
	);
	return data;
}

inline unsigned short inw(unsigned short _port)
{
	unsigned short data;
	asm volatile("inw ax, dx"
		: "=a" (data)
		: "d" (_port)
	);
	return data;
}

void NMIEnable(void)
/// The keyboard controller is an interesting microchip, read more about
/// it in the documentation.
// Port 0x70 is the keyboard controller port, first we read the current
// bit-mask and then we bitwise AND 0x7F to disable the NMI mask.
{
	outb(0x70, inb(0x70)&0x7F);
}

void NMIDisable(void)
// OR'ing bitwise 80 will turn on the bitmask for masking the NMI.
{
	outb(0x70, inb(0x70)|0x80);
}

inline void cli(void)
{
	asm volatile("cli"
		:
		:
		: "memory"		// Keep this exactly where we put it.
	);
}
inline void sti(void)
{
	asm volatile("sti"
		:
		:
		: "memory"
	);
}
