/*
 * (c) 2016 Apollo Project Developers
 * arch/x86/interrupts.c - CPU side of x86 Interrupts
 */

#include <arch/x86/idt.h>
#include <sys/console.h>
#include <arch/x86/pic.h>

static const unsigned short masterPIC = 0x20;
static const unsigned short slavePIC = 0xA0;

void encodeIDTEntry(IDTEntry *ptr, uint32_t base, uint16_t sel, uint8_t flags)
{
    ptr->baseLow   = base & 0xFFFF;
    ptr->baseHigh  = (base >> 16) & 0xFFFF;
    ptr->selector  = sel;
    ptr->zero      = 0;
    ptr->typeAttr  = flags;
}

void irqHandler(struct registers *regs)
{
    // TODO: handle Spurrious interrupts
    // We first acknowledge the interrupt was received, then we spin up the service handler for it
    if (regs->intNum >= 0x28) // Did this come from the slave PIC?
    {
        PIC_sendEOI(slavePIC);
    }
    // Always send an EOI to the Master PIC
    PIC_sendEOI(masterPIC);
    // Pass back to ISR handler now
}

void isrHandler(struct registers *regs)
{
    // Is this an IRQ?
    if(regs->intNum >= 0x20 && regs->intNum < 0x30) {
        irqHandler(regs);
    }
    console_write("Interrupt Received: ");
    console_write_hex(regs->intNum);
    // TODO: actual passoff to a function that cares!
}

void idtInit()
{
    static IDTEntry    idt[256];
    static IDTPointer  idtPtr;
    int i;
    uint32_t temp;

    // First we turn off the interrupts on the PIC's
    // We can use the same loop for both PIC's here
    for(i=0; i<8; i++) {
        PIC_set_mask((unsigned char)i, masterPIC);
        PIC_set_mask((unsigned char)i, slavePIC);
    }
    // Now we have to unmask master pic line 2,
    // That way when we enable lines later the slave pic can interrupt on it.
    PIC_clear_mask(0x02, masterPIC);

    // Now initialization time!
    // This is a long function!
    // Arguments in order:
    // arg1 : Set up init mode, and tell PIC we need ICW4 for init
    // arg2 : icw3 - let the PIC know it has a slave on line 2 (00000100)
    // arg3 : icw4 - We're in 8086 mode, nothing else needed :D
    // arg4 : offset - where we want the IRQ's to fire (0x20 is a safe bet)
    // arg5 : port - the IO port address of the PIC (coincidentally also 0x20)
    PIC_init(PIC_ICW1_INIT | PIC_ICW1_ICW4, 0x04,
            PIC_ICW4_8086, 0x20, masterPIC);

    // Same as before except:
    // arg2 : We're the slave pic, Master is listening to us at line 2
    // arg4 : offset of master + 8, so we don't step on it's toes
    // arg5 : IO port of slave is 0xA0 this time!
    PIC_init(PIC_ICW1_INIT | PIC_ICW1_ICW4, 0x02,
            PIC_ICW4_8086, 0x28, slavePIC);

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
