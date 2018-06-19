/*
 * (c) 2016 Apollo Project Developers
 * Main.c - Main function of kernel activity
 */

#include <sys/hal.h>
#include <string.h>

// These are defined by the linker from the "modules" section
extern module_t __start_modules, __stop_modules;

// Function definitions that are only used inside main. (these will be moved)
static void earlypanic(const char *msg, const char *msg2);
static module_t *find_module(const char *name);
static void resolve_module(module_t *m);
static void init_module(module_t *m);
static void fini_module(module_t *m);
static void log_status(int status, const char *name, const char *text);

// This variable will be overridden if we are in a test harness.
module_t *test_module __attribute__((weak)) = (module_t*)NULL;

int main(int argc, char **argv)
{
    // Set all module states to not initialised so the dependency tree works
    for(module_t *m = &__start_modules, *e = &__stop_modules; m < e; m++) {
        m->state = MODULE_NOT_INITIALISED;
    }
    // Resolve prereqs of the module.

    for(module_t *m = &__start_modules, *e = &__stop_modules; m < e; m++) {
        resolve_module(m);
    }

    // Init the console first, that way we have output.
    module_t *m = find_module("console");
    if(m) {
        init_module(m);
    }
    // If we're in a test harness, only load the test module dependencies
    if(test_module) {
        init_module(test_module);
    } else {
        for(module_t *m = &__start_modules, *e = &__stop_modules; m < e; m++) {
            init_module(m);
        }
        // Finally, run the main function, unless we're in test harness mode.

        write_console("Init'd",6);
        for(;;);
        kmain(argc, argv);
    }

    // We've returned from kernel, that means we're shutting down.
    // Run the finishing modules.
    for(module_t *m = &__start_modules, *e = &__stop_modules; m < e; m++) {
        fini_module(m);
    }
    return 0;
}

static void resolve_module(module_t *m)
{
    // Did we already resolve the pre-requisites?
    if (m->state >= MODULE_PREREQS_RESOLVED) {
        return;
    }
    // Find the required modules first
    for (prereq_t *p = m->required; p != NULL && p->name != NULL; p++) {
        p->module = find_module(p->name);
    }
    // Find the "soft" requirements next.
    for (prereq_t *p = m->load_after; p != NULL && p->name != NULL; p++) {
        p->module = find_module(p->name);
    }
    // Finally change state to show prereqs resolved.
    m->state = MODULE_PREREQS_RESOLVED;
}

// Initialize the module!
static void init_module(module_t *m)
{
    if (m->state >= MODULE_INIT_RUN) {
        return;
    }
    m->state = MODULE_INIT_RUN;

    if (m->required) {
        for (prereq_t *p = m->required; p != NULL && p->name != NULL; p++) {
            if (!p->module) {
                earlypanic("Module not found: ", p->name);
            } else {
                init_module(p->module);
            }
        }
    }
    if (m->load_after) {
        for (prereq_t *p = m->load_after; p != NULL && p->name != NULL; p++) {
            if (p->module) {
                init_module(p->module);
            }
        }
    }
    if (m->init) {
        int ok = m->init();
        log_status(ok, m->name, "Started");
    }
}

static void fini_module(module_t *m)
{
    if (m->state != MODULE_INIT_RUN) {
        return;
    }
    m->state = MODULE_FINI_RUN;

    if (m->required) {
        for (prereq_t *p = m->required; p != NULL && p->name != NULL; p++) {
            if(!p->module) {
                earlypanic("Module not found: ", p->name);
            } else {
                fini_module(p->module);
            }
        }
    }

    if (m->load_after) {
        for (prereq_t *p = m->load_after; p != NULL && p->name != NULL; p++) {
            if(p->module) {
                fini_module(p->module);
            }
        }
    }

    if (m->fini) {
        int ok = m->fini();
        log_status(ok, m->name, "Stopped");
    }
}

static module_t *find_module(const char *name)
{
    for(module_t *i = &__start_modules, *e = &__stop_modules; i < e; i++) {
        if (!strcmp(name, i->name)) {
            return i;
        }
    }
    return NULL;
}

static void log_status(int status, const char *name, const char *text) {
    write_console("[",1);
    if (status == 0) {
        write_console("\033[32m OK \033[0m", 13);
    } else {
        write_console("\033[31mFAIL\022[0m", 13);
    }
    write_console("] ", 2);
    write_console(text, strlen(text));
    write_console(" ", 1);
    write_console(name, strlen(name));
    write_console("\n", 1);
#ifdef HOSTED
    printf("main: %s %s with status %d\n", text, name, status);
#endif
}

static void earlypanic(const char *msg, const char *msg2) {
    write_console("PANIC! ", 7);
    write_console(msg, strlen(msg));
    if(msg2) {
        write_console(msg2, strlen(msg2));
    }
    write_console("\n", 1);

#ifdef HOSTED
    printf("main: PANIC! %s %s\n", msg, msg2);
#endif
    for (;;);
}

