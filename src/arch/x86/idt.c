/*
 * (oc) 2018 Apollo Developers
 * idt.c - Interrupt Descriptor Table
 */
#include <sys/hal.h>
#include <arch/x86/idt.h>
#include <stdio.h>

extern void idt_flush(uint32_t);

#define NUM_TRAP_STRS 20 	// x86 has 20 hardware traps
#define NUM_HANDLERS 256    // Max handlers x86 allows
#define MAX_HANDLERS_PER_INT 4  // Number of software handlers per interrupt

static const char *trap_strs[NUM_TRAP_STRS] = {
	"Division error",
	"Reserved",
	"Non-Maskable Interrupt",
	"Breakpoint",
	"Overflow",
	"BOUND range exceeded",
	"Invalid opcode",
	"Device not availible (Coprocessor Error)",
	"Double Fault",
	"Coprocessor segment overrun",
	"Bad TSS",
	"Segment not present",
	"Stack-segment fault",
	"General protection fault",
	"Page fault",
	"Reserved",
	"x87 FPU floating-point error",
	"Alignment check exception",
	"Machine check exception",
	"SIMD floating-point exception"
};

static struct {
    interrupt_handler_t handler;
    void *p;
} handlers[NUM_HANDLERS][MAX_HANDLERS_PER_INT];

// Allows for counting of handlers.
unsigned num_handlers[NUM_HANDLERS];

idt_entry_t idt[NUM_HANDLERS];
idt_ptr_t   idt_ptr;


// IRQ stuff. Implemented in another file.
void (*ack_irq)(unsigned) = 0;
void (*enable_irq)(uint8_t, unsigned) = 0;
int init_irq();
void enable_interrupts()
{
    __asm__ volatile("sti");
}

void disable_interrupts()
{
   __asm__ volatile("cli");
}

int get_interrupt_state()
{
    uint32_t flags;
    __asm__ volatile ( "pushf \n\t" "pop %0" : "=g"(flags) );
    return flags & (1 << 9); // Return the interrupt enabled flag
}

void set_interrupt_state(int enable)
{
    if(enable) {
        enable_interrupts();
    } else {
        disable_interrupts();
    }
}

void trap() {
    __asm__ volatile("int 0x03");
}

/**
 ** Debugging Functions
 **/

static void print_handlers(const char *cmd,core_debug_state_t *states,int core)
{
    unsigned i, j;

    for (i=0; i<NUM_HANDLERS; i++) {
        if(num_handlers[i] == 0) {
            continue;
        }
        printf("#%02d: ", i);                   // Handler Number
        for (j = 0; j < num_handlers[i]; j++) {
            interrupt_handler_t h = handlers[i][j].handler;
            int offs;
            const char *sym = lookup_kernel_symbol((uintptr_t)h, &offs);
            if (sym) {
                printf("%s+%#x ", sym, offs);   // symbol name, hex offset
            } else {
                printf("%p ", h);   // Pointer address of the kernel symbol
            }
        }
        printf("\n");               // Newline
    }
}

static void print_idt_entry(unsigned i, idt_entry_t e)
{
    if (i % 2) {
        printf("\n"); // Lazy way of double columning
    }

    printf("#%02d: Base %#08x ", i, e.base_low | (e.base_high<<16));
    printf("Sel: %#04x\t",  e.selector);
}

static void print_idt(const char *cmd, core_debug_state_t *states, int code)
{
    unsigned i;
    for (i = 0; i < NUM_HANDLERS; i++) {
        print_idt_entry(i, idt[i]);
    }
}

/**
 ** IDT initilization Functions
 **/

static void set_idt_entry(uint8_t n, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt[n].base_low = base & 0xFFFF;
    idt[n].base_high= (base >> 16) & 0xFFFF;
    idt[n].selector = sel;
    idt[n].always0  = 0;
    idt[n].flags    = flags;
}

static int init_idt()
{
    uint32_t i, delta;

    delta = (uint32_t)isr1 - (uint32_t)isr0; // Get size of each handler

    idt_ptr.limit   = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base    = (uint32_t) &idt;
    memset(&idt, 0, sizeof(idt_entry_t) * 265);

    for (i = 0; i < 256; i++) {
        // 0x08 is the Code segment
        // IDT_INT32_PL0 is the flags (see <arch/x86/idt.h>
        set_idt_entry(i, (uint32_t)isr0 + (delta * i), 0x08, 0x8E);
    }

    idt_flush((uint32_t)&idt_ptr);

    register_debugger_handler("print-idt", "Print the IDT", &print_idt);
    register_debugger_handler("print-interrupt-handlers",
        "Print all registered interrupt handlers", &print_handlers);
    init_irq();
    set_interrupt_state(1);
    return 0;
}

/**
 ** Handler Registration Functions
 **/

int register_interrupt_handler(int num, interrupt_handler_t handler, void *p)
{
    if (num >= NUM_HANDLERS) {
        return -1; // Interrupt line doesn't exist
    }
    if (num_handlers[num] >= MAX_HANDLERS_PER_INT) {
        return -1; // Too Many handlers, can't add
    }
    // I'm so sorry this looks like this.
    handlers[num][num_handlers[num]].handler = handler;
    handlers[num][num_handlers[num]++].p = p;

    if (num >= 32 && enable_irq) {
        enable_irq(num-32, 1);  // Enable the IRQ line if needed.
    }
    return 0;
}

int unregister_interrupt_handler(int num, interrupt_handler_t handler, void *p)
{
    int found = 0, i, e;
    if (num >= NUM_HANDLERS) {
        return -1;  // Interrupt line doesn't exist
    }

    for (i = 0, e = num_handlers[num]; i < e; i++) {
        if (handlers[num][i].handler == handler &&
            handlers[num][i].p == p) {
            found = 1;
        }
        if (found && i != e-1) {
            handlers[num][i] = handlers[num][i+1];
        }
    }
    if (found) {
        num_handlers[num]--;
        if (num_handlers[num] == 0 && num >= 32 && enable_irq) {
            enable_irq(num-32, 0);
        }
        return 0;
    }
    return 1;
}

void interrupt_handler(x86_regs_t *regs)
{
    unsigned num = regs->interrupt_num;
    unsigned i, e;
    const char *desc = "";
    char buf[64];


    // Acknowledge any IRQ lines
    if(ack_irq) {
        ack_irq(num);
    }

    if (num_handlers[num]) {
        for (i = 0, e = num_handlers[num]; i != e; i++) {
            handlers[num][i].handler(regs, handlers[num][i].p);
        }
    } else if (num == 3) {
        debugger_trap(regs);
    } else {
        if (regs->interrupt_num < NUM_TRAP_STRS) {
            desc = trap_strs[num];
        } else {
            sprintf(buf, "Exception %#04x", regs->interrupt_num);
            desc = buf;
        }
        debugger_except(regs, desc);
    }
}

static prereq_t prereqs[] = { {"x86/gdt",NULL}, {NULL,NULL} };
static prereq_t load_after[] = { {"debugger",NULL}, {NULL,NULL} };

MODULE = {
    .name = "interrupts",
    .required = prereqs,
    .load_after = load_after,
    .init = &init_idt,
    .fini = NULL
};
