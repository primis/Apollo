/*
 * (c) 2018 Apollo Project Developers
 * arch/x86/descriptors.c - GDT & TSS
 */


#include <sys/hal.h>
#include <arch/x86/gdt.h>
#include <stdio.h>
#include <string.h>


static gdt_ptr_t gdt_ptr;
static gdt_entry_t entries[MAX_CORES+5];
static tss_entry_t tss_entries[MAX_CORES];

unsigned num_gdt_entries, num_tss_entries;

static void set_gdt_entry(gdt_entry_t *e, uint32_t base, uint32_t limit,
                          uint8_t type, uint8_t s, uint8_t dpl, uint8_t p,
                          uint8_t l, uint8_t d, uint8_t g)
{
  e->limit_low  = limit & 0xFFFF;
  e->base_low   = base & 0xFFFF;
  e->base_mid   = (base >> 16) & 0xFF;
  e->type       = type & 0xF;
  e->s          = s & 0x1;
  e->dpl        = dpl & 0x3;
  e->p          = p & 0x1;
  e->limit_high = (limit >> 16) & 0xF;
  e->avail      = 0;
  e->l          = l & 0x1;
  e->d          = d & 0x1;
  e->g          = g & 0x1;
  e->base_high  = (base >> 24) & 0xFF;
}

static void set_tss_entry(tss_entry_t *e)
{
    // Zero out the entry.
    memset((uint8_t*)e, 0, sizeof(tss_entry_t));
    // Set the segment registers to the ring 0 data (0x10)
    e->ss0 = e->ss = e->ds = e->es = e->fs = e->gs = 0x10;
    // Set code segment to ring 0 code (0x08)
    e->cs = 0x08;
}

// Returns the base address of a gdt entry.
static uint32_t base(gdt_entry_t e) {
    return e.base_low | (e.base_mid << 16) | (e.base_high << 24);
}

// Returns the unobfuscated limit of a gdt entry.
static uint32_t limit(gdt_entry_t e) {
    return e.limit_low | (e.limit_high << 16);
}

static void print_gdt_entry(unsigned i, gdt_entry_t e)
{
    uint32_t *m = (uint32_t*) &e;
    printf("#%02d: %08x %08x\n", i, m[0], m[1]);
    printf("#%02d: Base %#08x Limit %#08x Type %d\n",
            i, base(e), limit(e), e.type);
    printf("     s %d dpl %d p %d l %d d %d g %d\n",
            e.s, e.dpl, e.p, e.l, e.d, e.g);
}

static void print_tss_entry(unsigned i, tss_entry_t e) {
    printf("#%02d: esp0 %#08x ss0 %#02x cs %#02x\n"
            "ss %#02x ds %#02x es %#02x fs %#02x gs %#02x\n",
            i, e.esp0, e.ss0, e.cs, e.ss, e.ds, e.es, e.fs, e.gs);
}

static void print_tss(const char *cmd, core_debug_state_t *states, int core)
{
    unsigned i;
    for(i = 0; i < num_tss_entries; i++) {
        print_tss_entry(i, tss_entries[i]);
    }
}

static void print_gdt(const char *cmd, core_debug_state_t *states, int core)
{
    unsigned i;
    for(i = 0; i < num_gdt_entries; i++) {
        print_gdt_entry(i, entries[i]);
    }
}

static int gdt_init()
{
    int i;

    register_debugger_handler("print-gdt", "Print GDT Entries", &print_gdt);
    register_debugger_handler("print-tss", "Print TSS Entries", &print_tss);

    // Ring 0 and Ring 3 data/code gdt entries for full memory size
    // ~0u is shorthand for 0xFFFFFFFF (1's complement on unsigned 0)
    //                          Base  Limit   Type            S DPL P  L  D  G
    set_gdt_entry(&entries[0],0xFFF0, 0,                   0, 0, 0, 0, 0, 0, 0);
    set_gdt_entry(&entries[1],   0, ~0u, TY_CODE|TY_READABLE, 1, 0, 1, 0, 1, 1);
    set_gdt_entry(&entries[2],   0, ~0u, TY_DATA_WRITABLE   , 1, 0, 1, 0, 1, 1);
    set_gdt_entry(&entries[3],   0, ~0u, TY_CODE|TY_READABLE, 1, 3, 1, 0, 1, 1);
    set_gdt_entry(&entries[4],   0, ~0u, TY_DATA_WRITABLE   , 1, 3, 1, 0, 1, 1);

    int num_processors = get_num_processors();
    if (num_processors == -1) {
        num_processors = 1; // If get_num_processors hasn't been implemented.
    }
    for(i = 0; i < num_processors; i++) {
        set_tss_entry(&tss_entries[i]);
        set_gdt_entry(&entries[i+5], (uint32_t)&tss_entries[i],
                //                                   Type   S DPL P  L  D  G
                sizeof(tss_entry_t)-1, TY_CODE|TY_ACCESSED, 0, 3, 1, 0, 0, 1);
    }
    num_gdt_entries = num_processors + 5;
    num_tss_entries = num_processors;

    // Set up the pointer for the GDT
    gdt_ptr.base    = (uint32_t)&entries[0];
    gdt_ptr.limit   = sizeof(gdt_entry_t) * num_gdt_entries - 1;

    __asm__ volatile(   "lgdt %0;"
                        "mov ax, 0x10;"
                        "mov ds, ax;"
                        "mov es, ax;"
                        "mov fs, ax;"
                        "mov gs, ax;"
                        "ljmp 0x08:$+7;"
                        "1:" : : "m" (gdt_ptr) : "eax");
    return 0;
}

static prereq_t prereqs[] = {{"debugger",NULL}, {"console",NULL}, {NULL,NULL}};

MODULE = {
    .name       = "x86/gdt",
    .required   = NULL,
    .load_after = prereqs,
    .init       = &gdt_init,
    .fini       = NULL
};
