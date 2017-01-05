/*
 * (c) 2016 Apollo Project Developers
 * Main.c - Main function of kernel activity
 */
#include <sys/init.h>
#include <sys/console.h>
int main()
{
    archInit();
    console_write("test\n");
    return 0xDEADC0DE;
}
