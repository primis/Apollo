/*
 * (c) 2018 Apollo Developers
 * testing.c - Testing harness interface
 */

#include <sys/hal.h>
#include <sys/testing.h>

// Symbols defined by the linker. Essentially an array of test functions!
extern test_mod_t __start_testing, __stop_testing;

static void run_all_tests()
{
    test_mod_t *m, *e = &__stop_testing;
    for (m = &__start_testing; m < e; m++) {
        m->runner(); // Run the test
    }
}


static int testing()
{
    // Making a fake argc.
    const char *argv[] = {"test", "-v"};

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

