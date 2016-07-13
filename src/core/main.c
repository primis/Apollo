/*
 * (c) 2016 Apollo Developers
 * main.c - The Opening point of the Kernel Proper.
 */

#include <sys/init.h>
void initModules()
{
    vgaInit(0x3D5, 0x3D4, 0xB8000);             // Index, Data, Base Address.
    scrWrite("Initializing the GDT...");
    gdtInit();
    scrWrite("[ OK ]\n");

    scrWrite("Initializing The IDT...");
    idtInit();                                  // IDT takes no Arguments.
    scrWrite("[ OK ]\n");

    scrWrite("Initializing The PIC...");
    picInit(0x20, 0xA0);                        // Command Registers of the PIC
    scrWrite("[ OK ]\n");

    scrWrite("Initializing the PIT...");
    pitInit(CLOCKS_PER_SEC);
    scrWrite("[ OK ]\n");
}

int main()
{
    initModules();

    // Show what git revision we've built.
    scrWrite("Apollo Kernel, Git Revision ");
    scrWrite(gitRevision);
    scrWrite("\n");
    scrWrite("Hello, Netbooting World! :D\n");
    while(1){
        ;
    }
    return 0xDEADC0DE;
}
