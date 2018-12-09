/*
 * (c) 2018 Apollo Project Developers
 * <hal.h> - Hardware Abstraction Layer
 *
 * Thank you to James Molloy for the concept
 */

#ifndef __HAL_H
#define __HAL_H

#include <types.h>

typedef struct spinlock {

    volatile unsigned val;
    volatile unsigned interrupts;
} spinlock_t;

// Platform Specific hal.h's
// Right now we only have one real one - x86

#if defined(X86)
#include "arch/x86/hal.h"
#elif defined(HOSTED)
#include "arch/hosted/hal.h"
#else
#error Unknown Target. Please use an existing target.
#endif


// Check for test harness
#ifndef TEST_HARNESS
#define TEST_HARNESS 0
#endif


// hal.h is going to have a lot of functions.

// System Panic and Assert
void panic(const char *message) __attribute__((noreturn));
void assert_fail(const char *cond, const char *file, int line)__attribute__((noreturn));

// Kernel Main (aka kmain)
void kmain(int argc, char **argv);


/*****************************************************************************
 * Module States and configuration
 *****************************************************************************/

// Module states - Used by main to determine if module is ready
enum module_state {
    MODULE_NOT_INITIALISED,
    MODULE_PREREQS_RESOLVED,
    MODULE_INIT_RUN,
    MODULE_FINI_RUN
};

// Module Prerequesites - Used to determine if the module should be run yet.
// Individual Modules should only fill in 'name'. The rest is handled by
// the kernel.
typedef struct prereq {
    const char *name;
    struct module *module;
} prereq_t;

// Definition of a module.
// Includes both construction (init) and deconstruction (fini) definitions
// Only need to really define one, but it's nice to have both.
typedef struct module {
    const char *name;       // Null terminated unique identifier
    prereq_t *required;     // Either NULL or Null terminated list
                            // of required modules to run before this one
    prereq_t *load_after;   // Same as above, but non-hard prerequisites.
                            // These are modules that we'd "like" to be
                            // loaded first
    int (*init)(void);      // Startup function. Return 0 for success
    int (*fini)(void);      // Deconstructor / shutdown function. Return 0
    uintptr_t state;
    uintptr_t padding[2];   // Alignment.
} module_t;

#define MODULE static module_t x __attribute__((__section__("modules"),used))
#define run_test static module_t x __attribute__((__section__("testing"),used))

/******************************************************************************
 * Console funtionality
 *****************************************************************************/

typedef struct console {
    int (*open)(struct console *obj);   // Init a console (screen, serial, etc)
    int (*close)(struct console *obj);  // Close a console (finished with it)

    // Read up to len bytes from console. return 0 for no data.
    // Return -1 on error, Return num of bytes on success.
    int (*read)(struct console *obj, char *buf, int len);
    // Write len bytes to console. Return -1 on failure.
    int (*write)(struct console *obj, const char *buf, int len);
    // Flush write buffer for a console.
    void (*flush)(struct console *obj);

    // Linked list, internal use only.
    struct console *prev, *next;
    // Implementation specific datum, for use for whatever
    void *data;
} console_t;

int register_console(console_t *c);
void unregister_console(console_t *c);
// Write to all consoles registered
void write_console(const char *buf, int len);
// Read from default console
int read_console(char *buf, int len);

/*****************************************************************************
 *  Interrupt handling
 ****************************************************************************/

// Callback type for an interrupt handler. Takes a pointer to target specific
// 'regs' containing register contents. Returns nonzero if any changes were
// made to 'r' that should be reflected when handler returns.
// 'p' is opaque data passed from register_interrupt_handler.

struct regs;
typedef int (*interrupt_handler_t)(struct regs *r, void *p);

// Register a new interrupt handler on given interrupt 'num'. There can be more
// than one interrupt hander per interrupt.

int register_interrupt_handler(int num, interrupt_handler_t h, void *p);
int unregister_interrupt_handler(int num, interrupt_handler_t h, void *p);
void enable_interrupts();
void disable_interrupts();
int get_interrupt_state();  // Returns 1 if enabled, 0 if disabled.
void set_interrupt_state(int enable);

/*****************************************************************************
 * Debugging
 ****************************************************************************/

// 256 is enough, right? Yeah, that's good.

#define MAX_CORES 256

typedef struct core_debug_state {
    struct regs *registers;
} core_debug_state_t;

// Debug handler. 'core' is current processor.
typedef void (*debugger_fn_t)(const char *cmd,
        core_debug_state_t *states, int core);
// Cause a debug or breakpoint trap.
void trap();
// Invoke the debugger from a debug trap interrupt handler.
void debugger_trap(struct regs *regs);
// Invoke the debugger from an interrupt handler that isn't working correctly
void debugger_except(struct regs *regs, const char *description);
// Register a function for the debugger
int register_debugger_handler(const char *name, const char *help,
        debugger_fn_t fn);
// Perform a backtrace.
// If called with *data=0, return the IP location of current function's caller
// Iteratively call until return value is 0, at which point backtrace is done.
uintptr_t backtrace(uintptr_t *data, struct regs *regs);

// Set an instruction breakpoint, returns -2 if not possible on target, -1
// for a normal error. Otherwise the return is implementation specific.
int set_insn_breakpoint(uintptr_t loc);
// id is the number returned from the "set" of the breakpoint.
// returns -2 if not supported, -1 error, 0 success
int unset_insn_breakpoint(int id);

// Data read breakpoints. Same as above with implementation.
int set_read_breakpoint(uintptr_t loc);
int unset_read_breakpoint(int id);

// Data write breakpoints. Same as above with implementation.
int set_write_breakpoint(uintptr_t loc);
int unset_write_breakpoint(int id);

// Return a string giving the name of the symbol that addr is in.
// Store the offset from that symbol in *offs.
// Returns NULL if not supported or symbol is unavailible.

const char *lookup_kernel_symbol(uintptr_t addr, int *offs);

// Given an implementation defined 'regs',
// fill in given array of name-value pairs
int describe_regs(struct regs *regs, int max, const char **names,
        uintptr_t *values);

/****************************************************************************
 * Multiple Processors
 ***************************************************************************/

// Returns -1 if not implemented
int get_processor_id();

// Returns -1 if not implemented
int get_num_processors();

// Returns pointer to array of all Processor ID's
int *get_all_processor_ids();

// Return implementation defined value to be passed to
// register_interrupt_handler to handle IPC's and interrupts.
// Returns -1 if not implemented

int get_ipi_interrupt_num();

// Given a struct regs from interrupt handler registered to
// get_ipi_interrupt_num, returns value that was passed to send_ipi function.
void *get_ipi_data(struct regs *r);

#define IPI_ALL -1
#define IPI_ALL_BUT_THIS -2

// Sends an inter-processor interrupt.
// Value of IPI_ALL will send IPI's to all cores
// Value of IPI_ALL_BUT_THIS will send IPI's to all cores but calling one.
void send_ipi(int proc_id, void *data);


/*****************************************************************************
 * Peripherals
 ****************************************************************************/

uint64_t get_timestamp();
void set_timestamp(uint64_t ts);

// Registers a callback to be fired in 'num_millis' milliseconds. If periodic
// is nonzerp, call repeatedly on delay, else call once.

int register_callback(uint32_t num_millis, int periodic, void(*cb)(void*),
        void *data);

int unregister_callback(void(*cb)(void*));

/*****************************************************************************
 * Memory Management
 ****************************************************************************/

#define PAGE_WRITE   1 // Page is writable
#define PAGE_EXECUTE 2 // Page is executable
#define PAGE_USER    4 // Page is useable by user mode code (else kernel only)
#define PAGE_COW     8 // Page is marked copy-on-write. It must be copied if
                       // written to.

#define PAGE_REQ_NONE     0 // No requirements on page location
#define PAGE_REQ_UNDER1MB 1 // Require that the returned page be < 0x100000
#define PAGE_REQ_UNDER4GB 2 // Require that the returned page be < 0x10000000

unsigned get_page_size();

// Rounds an address to a page alinged one.
uintptr_t rount_to_page_size(uintptr_t x);


// Allocate a physical page of size returned by get_page_size()
// This should only be used between calling init_physical_memory_early()
// and init_physical_memory().
uint64_t early_alloc_page();

// Allocate a physical page. Returns NULL on failure.
// 'req' is a 'PAGE_REQ_*' flag, indicating a type of page needed.

uint64_t alloc_page(int req);
// Returns -1 on failure
int free_page(uint64_t page);

uint64_t alloc_pages(int req, size_t num);
int free_pages(uint64_t pages, size_t num);

// Creates a new address space based on current one and stores it in 'dest'.
// if 'make_cow' is non_zero, all WRITE pages are modified to copy-on-write
int clone_address_space(address_space_t *dest, int make_cow);

// Returns -1 on failure.
int switch_address_space(address_space_t *dest);

// Returns current address space.
address_space_t *get_current_address_space();

// Mapes num_pages * get_page_size() bytes from p in physical address space to
// v in current virtual address space.
// returns 0 on success or -1 on failure.

int map(uintptr_t v, uint64_t p, int num_pages, unsigned flags);
int unmap(uintptr_t v, int num_pages);

// If v has a V-P mapping, return 'v' else return next page which has a mapping
// associated with it.

uintptr_t iterative_mappings(uintptr_t v);

// If v is mapped, return physical page it is maapped to and fill 'flags' with
// the mapping flags. Otherwise returns NULL

uint64_t get_mapping(uintptr_t v, unsigned *flags);

// return 1 if 'v' is mapped, 0 if not, -1 if not implemented.
int is_mapped(uintptr_t v);

// A range of memory, with a start and size.

typedef struct range {
    uint64_t start;
    uint64_t extent;
} range_t;

// Initialise the virtual memory manager.

int init_virtual_memory();

// Stages of memory management initialisation.
#define PMM_INIT_START 0
#define PMM_INIT_EARLY 1
#define PMM_INIT_FULL  2

// Variable of the current state of physical memory management.
extern unsigned pmm_init_stage;

// Init the physical mem manager to stage 1, passing in a set of ranges
// and the max extent of physical memory.
int init_physical_memory_early(range_t *ranges, unsigned nrages,
        uint64_t extent);

// Init the physical memory manager to stage 2. This should be done after
// Virtual memory manager is set up.

int init_physical_memory();

// Init the copy on write page reference counts.
int init_cow_refcnts(range_t *ranges, unsigned nranges);

// Increment the refrence count of a COW page.
void cow_refcnt_inc(uint64_t p);
void cow_refcnt_dec(uint64_t p);
// Return the reference count of a COW page.
unsigned cow_refcnt(uint64_t p);

// Handle a page fault potentially caused by a COW access.
bool cow_handle_page_fault(uintptr_t addr, uintptr_t error_code);

/*****************************************************************************
 * Devices
 ****************************************************************************/

typedef uint32_t dev_t;

// Character stream device.
typedef struct char_device{
  int (*read)(struct char_device *obj, char *buf, uint64_t len);
  int (*write)(struct char_device *obj, char *buf, uint64_t len);
  void (*flush)(struct char_device *obj);
  /* Register a callback which will fire when the device has data available. */
  int (*register_callback)(struct char_device *obj, void (*cb)(void*),
                           void *cb_param);
  int (*unregister_callback)(struct char_device *obj, void (*cb)(void*),
                             void *cb_param);
  // Return a string describing the device.
  void (*describe)(struct char_device *obj, char *buf, unsigned bufsz);
  dev_t id;
  void *data;
} char_device_t;

// A block device - a random access device.
typedef struct block_device {
  int (*read)(struct block_device *o, uint64_t offset, void *buf, uint64_t len);
  int (*write)(struct block_device *o, uint64_t offset, void *buf,uint64_t len);
  void (*flush)(struct block_device *o);
  uint64_t (*length)(struct block_device *o);
  // Return a string describing the device.
  void (*describe)(struct block_device *o, char *buf, unsigned bufsz);
  dev_t id;
  void *data;
} block_device_t;

// Known device major numbers.
#define DEV_MAJ_NULL 0
#define DEV_MAJ_ZERO 1
#define DEV_MAJ_HDA  2
#define DEV_MAJ_HDB  3
#define DEV_MAJ_HDC  4
#define DEV_MAJ_HDD  5
#define DEV_MAJ_SDA  6
#define DEV_MAJ_SDB  7
#define DEV_MAJ_SDC  8
#define DEV_MAJ_SDD  9
static inline unsigned minor(dev_t x) {
  return x & 0xFFFF;
}
static inline unsigned major(dev_t x) {
  return (x >> 16) & 0xFFFF;
}
static inline dev_t makedev(unsigned major, unsigned minor) {
  return ((major&0xFFFF) << 16) | (minor & 0xFFFF);
}

int register_char_device(dev_t id, char_device_t *dev);
int register_block_device(dev_t id, block_device_t *dev);
void *unregister_device(dev_t id);
char_device_t *get_char_device(dev_t id);
block_device_t *get_block_device(dev_t id);
int register_block_device_listener(void (*callback)(dev_t));

/*******************************************************************************
 * Threading
 ******************************************************************************/

#define SPINLOCK_RELEASED {.val=0, .interrupts=0};
#define SPINLOCK_ACQUIRED {.val=1, .interrupts=0};

void spinlock_init(spinlock_t *lock);
spinlock_t *spinlock_new();
void spinlock_acquire(spinlock_t *lock);
void spinlock_release(spinlock_t *lock);

typedef struct semaphore {
  volatile unsigned val;

  spinlock_t queue_lock;
  struct thread *queue_head;
} semaphore_t;

// Initialise a semaphore to the value zero.
void semaphore_init(semaphore_t *s);
// Returns a new semaphore, initialised to the value zero.
semaphore_t *semaphore_new();
// Reduce the semaphore by one - pend/wait/acquire. This may be a blocking
// operation.
void semaphore_wait(semaphore_t *s);
// Increase the semaphore by one - post/signal/release.
void semaphore_signal(semaphore_t *s);

typedef semaphore_t mutex_t;

// Initialise a mutex to released.
static inline void mutex_init(mutex_t *s) {
  semaphore_init(s);
  semaphore_signal(s);
}
// Returns a new mutex, initialised to released.
static inline mutex_t *mutex_new() {
  mutex_t *m = semaphore_new();
  semaphore_signal(m);
  return m;
}

// Acquire the mutex. Blocking operation.
static inline void mutex_acquire(mutex_t *s) {
  semaphore_wait(s);
}
// Release the mutex.
static inline void mutex_release(mutex_t *s) {
  semaphore_signal(s);
}

// A readers-writers lock: multiple readers, one writer.
typedef struct rwlock {
  semaphore_t r, w;
  spinlock_t lock;
  volatile unsigned readcount, writecount;
} rwlock_t;

// Initialise a readers-writers lock.
void rwlock_init(rwlock_t *l);
void rwlock_read_acquire(rwlock_t *l);
void rwlock_read_release(rwlock_t *l);
void rwlock_write_acquire(rwlock_t *l);
void rwlock_write_release(rwlock_t *l);

int setjmp(jmp_buf buf) __attribute__((returns_twice));
void longjmp(jmp_buf buf, int val);
void jmp_buf_to_regs(struct regs *r, jmp_buf buf);

#endif
