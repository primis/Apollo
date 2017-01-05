/*
 * (c) 2016 Apollo Project Developers
 * arch/x86/interrupts.c - CPU side of x86 Interrupts
 */

#include <arch/x86/idt.h>

void encodeIDTEntry(IDTEntry *ptr, uint32_t base, uint16_t sel, uint8_t flags)
{
    ptr->baseLow   = base & 0xFFFF;
    ptr->baseHigh  = (base >> 16) & 0xFFFF;
    ptr->selector  = sel;
    ptr->zero      = 0;
    ptr->typeAttr  = flags;
}

void isrHandler(struct registers *regs)
{
    /*/ Theoretically this shouldn't happen. We check anyway
    if(interruptHandlers[regs->intNum] != NULL) {
        voidCallback handler = interruptHandlers[regs->intNum];
        // Call the interrupt handler with argc (count) and regs (argv)
        handler(sizeof(struct registers)/sizeof(reg_t), (reg_t*)regs);
    }*/
    while(1);
}

void irqHandler(struct registers *regs)
{
    // TODO - Might be put in it's own file

}

void idtInit()
{
    static IDTEntry    idt[256];
    static IDTPointer  idtPtr;
    int i;
    uint32_t temp;

    // get distance between irq0 and irq1
    temp = (uint32_t)isr1 - (uint32_t)isr0;
    // Set up the IDT pointer for the CPU
    idtPtr.limit = sizeof(IDTEntry) * 256 - 1;
    idtPtr.base  = (uint32_t)&idt;

    // Clear them out so we don't get triple faults..
    memset(&idt, 0, sizeof(IDTEntry)*256);

    // Set up the IDT itself first
    for(i=0; i<256; i++) {
        encodeIDTEntry(&idt[i], (uint32_t)isr0+(temp*i), 0x08, IDT_INT32_PL0);
    }

    // Now we Set up service handlers
    for(i=0; i<256; i++) {
        interruptHandlerRegister(i, unhandledInterrupt);
    }

    // Finally, we can push changes to the CPU and enable interrupts
    idtFlush((uint32_t)&idtPtr);
}
