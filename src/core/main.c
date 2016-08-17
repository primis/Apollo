/*
 * (c) 2016 Apollo Project Developers
 * Main.c - Main function of kernel activity
 */
#include <sys/init.h>
int main()
{
    archInit();
    return 0xDEADC0DE;
}
