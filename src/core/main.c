/*
 * Copyright (c) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * Main.c - Main function of kernel activity
 */

#include <sys/hal.h>
#include <string.h>
#include <stdio.h>

// These are defined by the linker from the "modules" section
extern module_t __modules_begin, __modules_end;

// Function definitions that are only used inside main. (these will be moved)
static void earlypanic(const char *msg, const char *msg2);
static module_t *find_module(const char *name);
static void fini_module(module_t *m);
static void log_status(int status, const char *name, const char *text);

// TODO: put these in a header
void set_log_level(int);
void resolve_module(module_t *m);
void init_module(module_t *m);

// This variable will be overridden if we are in a test harness.
module_t *test_module __attribute__((weak)) = (module_t*)NULL;

static int logLevel;

static int search_argv(const char* arg, int argc, char **argv)
{
    int i;
    for(i = 0; i < argc; i++)
    {
        if(strcmp(argv[i], arg) == 0) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char* argv[])
{
    int log_level_setting = search_argv("debug=y", argc, argv);

    set_log_level(log_level_setting); // Turn on logging
    // Set all module states to not initialised so the dependency tree works
    for(module_t *m = &__modules_begin, *e = &__modules_end; m < e; m++) {
        m->state = MODULE_NOT_INITIALISED;
    }
    // Resolve prereqs of the module.

    for(module_t *m = &__modules_begin, *e = &__modules_end; m < e; m++) {
        resolve_module(m);
    }

    // Init the console first, that way we have output.

    module_t *console = find_module("console");
    if(console) {
        init_module(console);
    }

    for(module_t *m = &__modules_begin, *e = &__modules_end; m < e; m++) {
        if((m != test_module) && (m != console)) { // this should be done last!
            init_module(m);
        }
    }
    if(test_module && TEST_HARNESS == 1) {
        set_log_level(0);
        init_module(test_module);
    } else {
        enable_interrupts();
        kmain(argc, argv);
    }
    set_log_level(log_level_setting); // This could have been changed elsewhere.

    // We've returned from kernel, that means we're shutting down.
    // Run the finishing modules.
    for(module_t *m = &__modules_begin, *e = &__modules_end; m < e; m++) {
        fini_module(m);
    }
#ifndef HOSTED
    for(;;); // Spin loop
#endif
    return 0;
}

void set_log_level(int l)
{
    logLevel = l;
}

void resolve_module(module_t *m)
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
void init_module(module_t *m)
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
    } else {
        log_status(-1, m->name, "No init() found!");
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
    for(module_t *i = &__modules_begin, *e = &__modules_end; i < e; i++) {
        if (!strcmp(name, i->name)) {
            return i;
        }
    }
    return NULL;
}

static void log_status(int status, const char *name, const char *text) {
    if (logLevel == 0) {
        return; // No logging enabled
    }
    if (status == 0) {
        printf("[\033[32m OK \033[0m] ");
    } else {
        printf("[\033[31mFAIL\033[0m] ");
    }
    printf("%s %s\n", text, name);
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
