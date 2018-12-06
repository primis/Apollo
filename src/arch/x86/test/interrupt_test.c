#include <sys/hal.h>
#include <unity_fixture.h>

TEST_GROUP(Interrupts);

static volatile uint32_t testVar;

static int callback(struct regs *r, void *p)
{
    testVar = (uint32_t)p;
    return 0;
}

TEST_SETUP(Interrupts)
{
    testVar = 0;
}

TEST_TEAR_DOWN(Interrupts)
{
}

TEST(Interrupts, pointerDataTest)
{
    register_interrupt_handler(3, &callback, (void*)0xDEADBEEF);
    trap();
    TEST_ASSERT_EQUAL(0xDEADBEEF, testVar);
    unregister_interrupt_handler(3, &callback, (void*)0xDEADBEEF);
}

TEST_GROUP_RUNNER(Interrupts)
{
    RUN_TEST_CASE(Interrupts, pointerDataTest);
}

static void runTests()
{
    RUN_TEST_GROUP(Interrupts);
}
static int interrupt_test()
{
    return UnityMain(0, NULL, runTests);
}

static prereq_t p[] = { {"console", NULL}, {"interrupts", NULL}, {NULL, NULL} };

MODULE = {
    .name = "interrupt_test",
    .required = p,
    .load_after = NULL,
    .init = &interrupt_test,
    .fini = NULL
};
