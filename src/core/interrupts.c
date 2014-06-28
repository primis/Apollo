/*
 * (c) 2014 Apollo Developers 
 * interrupts.h - Uniform Interrupt Handler
 */

#include <sys/callback.h>
#include <sys/interrupt.h>
#include <sys/screen.h>

void_callback_arg_t interruptHandlers[256];

void interruptHandlerRegister(unsigned char _n, void_callback_arg_t _f)
/***********************************************************************
* Void_Callback_Arg_t is a function callback with the following format
* void fucntion(unsigned int a, ...);
* x86 calls the functions with the seven General Purpose Registers.
* Arguments: _n = number of interrupt to register
*            _f = function of type void_callback_arg_t
***********************************************************************/
{
	interruptHandlers[_n] = _f;		// Set function into the vector
}

void unhandledInterrupt(unsigned int _intNum, ...)
{
	scrWrite("Recieved An Unhandled Interrupt: ");
	scrWriteHex(_intNum);
	scrWrite("!\n");
}
