/*
 * (c) 2018 Apollo Developers
 * testing.c - Testing harness interface
 */


#include <sys/hal.h>
#include <unity_fixture.h>

// Defined in main.c
extern void resolve_module(module_t *m);
extern void init_module(module_t *m);
extern void set_log_level(int);

// Symbols defined by the linker. Essentially an array of test modules!
extern module_t __start_testing, __stop_testing;

static void run_all_tests()
{
    module_t *m, *e = &__stop_testing;
    for(m = &__start_testing; m < e; m++) {
        init_module(m);
    }
}


static int testing()
{
    // Making a fake argc so we can request verbosity.
    const char *argv[] = {"test", "-v"};

    module_t *m, *e = &__stop_testing;
    // Basically a copy of main.c, but just for tests
    for (m = &__start_testing; m < e; m++) {
        m->state = MODULE_NOT_INITIALISED;
    }
    // Resolve prerequesites of the tests
    for(m = &__start_testing; m < e; m++) {
        resolve_module(m);
    }
    // Run testsi
    return UnityMain(2, argv, run_all_tests);
}


// Basically load after any console device.
static prereq_t p[] = { {"console", NULL}, {"x86/screen",NULL},
                        {"hosted/console", NULL},
                        {"x86/serial",NULL},{NULL,NULL} };

MODULE = {
    .name = "testing",
    .required = NULL,
    .load_after = p,
    .init = &testing,
    .fini = NULL
};


// The MODULE macro creates a module_t type named x. here we assign it to the
// test_module pointer to fill the weakly assigned variable created in main.c
module_t *test_module = &x;

