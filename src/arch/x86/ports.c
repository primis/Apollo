/*
 * (c) 2018 Apollo Project Developers
 * ports.c - x86 I/O databus commends
 */


void outb(unsigned short port, unsigned char value)
{
    __asm__ __volatile__ ("outb %0, %1" : : "dN" (port), "a" (value));
}

unsigned char inb(unsigned short port)
{
    unsigned char ret;
    __asm__ __volatile__ ("inb %0, %1" : "=a" (ret) : "dN" (port));
    return ret;
}

unsigned short inw(unsigned short port)
{
    unsigned short ret;
    __asm__ __volatile__ ("inw %0, %1" : "=a" (ret) : "dN" (port));
    return ret;
}

void iowait(void)
{
    // Port 0x80 is used for POST checkpoints, so it's safe to use
    // As a "scrap" port to throw data to as a bus wait.
    // On a side note, port 0x80 shows up on certain MoBo's as a diagnostic
    // for POST Codes, so that's kinda cool.
    __asm__ __volatile__ ("outb 0x80, %0" : : "a" ((unsigned char)0));
}
