#include <sys/hal.h>
#include <unity_fixture.h>

TEST_GROUP(interrupt_test);

static volatile uint32_t testVar;

static int callback(struct regs *r, void *p)
{
    testVar = (uint32_t)p;
    return 0;
}

TEST_SETUP(interrupt_test)
{
    testVar = 0;
    register_interrupt_handler(3, &callback, (void*)0xDEADBEEF);
}
TEST_TEAR_DOWN(interrupt_test)
{
    unregister_interrupt_handler(3, &callback, (void*)0xDEADBEEF);
}


TEST(interrupt_test, pointerDataTest)
{
    trap();
    TEST_ASSERT_EQUAL(0xDEADBEEF, testVar);
}

TEST(interrupt_test, actuallyUnregisters)
{
    unregister_interrupt_handler(3, &callback, (void*)0xDEADBEEF);
    trap();
    TEST_ASSERT_EQUAL(0, testVar);
}

TEST(interrupt_test, TooManyHandlers)
{
    // This should fail after 4 registered
    // We already have 1 from the setup.
    int i = 1;
    while(register_interrupt_handler(3, &callback, (void*)i) == 0) {
        i++;
        if(i > 300) { // arbitrarily large number
            break;
        }
    }
    TEST_ASSERT_EQUAL(4, i);
    // unregister all the interrupts we assigned.
    while(unregister_interrupt_handler(3, &callback, (void*)i--) == 0);
}

static int run_tests()
{
    RUN_TEST_CASE(interrupt_test, pointerDataTest);
    RUN_TEST_CASE(interrupt_test, actuallyUnregisters);
    RUN_TEST_CASE(interrupt_test, TooManyHandlers);
    return 0;
}

static prereq_t p[] = { {"interrupts", NULL}, {NULL, NULL} };

run_test = {
    .name = "interrupt_test",
    .required = p,
    .load_after = NULL,
    .init = &run_tests,
    .fini = NULL
};
