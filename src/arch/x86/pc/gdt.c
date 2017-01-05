/*
 * (c) 2016 Apollo Project Developers
 * arch/x86/gdt.c - Initialization of the GDT on the x86
 */

#include <arch/x86/gdt.h>


/*
 * These have to be global because otherwise we couldn't modify the gdt later
 */
GDTEntry   GDT[5];
GDTPointer gdtPointer;

/**
 * encodeGDTEntry
 * Takes all the things you need from a GDT Entry and packs them into the
 * proper format for you.
 * NOTE:
 * uint8_t *gdt should be 8 bytes wide. Type definitions are in the header.
 */

void encodeGDTEntry(GDTEntry *ptr, uint32_t base, uint32_t limit, uint16_t type)
{
    uint8_t *gdt = (uint8_t *)ptr;
    // Sanity check for the limit
    if ((limit > 65536) && (limit & 0xFFF) != 0xFFF) {
        // Commented out due to lack of kerror.
        // kerror("Illegal limit in GDT");
    }

    // Type is nice and simple... Because we did the work in the header.
    gdt[5] = type & 0xFF;
    gdt[6] = (type >> 8) & 0xF0;
    // Encode the limit. Horribly convoluted.
    gdt[0] = limit & 0xFF;
    gdt[1] = (limit >> 8) & 0xFF;
    gdt[6] |= (limit >> 16) & 0xF;

    // Base is a bit better. Not in order mind you...
    gdt[2] = base         & 0xFF;
    gdt[3] = (base >> 8)  & 0xFF;
    gdt[4] = (base >> 16) & 0xFF;
    gdt[7] = (base >> 24) & 0xFF;

}

/**
 * gdtInit - Fill the GDT with the barebones for flat segmentation.
 */
void gdtInit()
{
    gdtPointer.limit = (sizeof(GDTEntry) * 5) - 1;
    gdtPointer.base  = (uint32_t *)GDT; // Actual Entries.
    encodeGDTEntry(&GDT[0], 0, 0, 0); // Null Selector
    encodeGDTEntry(&GDT[1], 0, 0xFFFFFFFF, GDT_CODE_PL0);
    encodeGDTEntry(&GDT[2], 0, 0xFFFFFFFF, GDT_DATA_PL0);
    encodeGDTEntry(&GDT[3], 0, 0xFFFFFFFF, GDT_CODE_PL3);
    encodeGDTEntry(&GDT[4], 0, 0xFFFFFFFF, GDT_DATA_PL3);
    gdtFlush((uint32_t)&gdtPointer);
}


