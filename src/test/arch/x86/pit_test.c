#include <sys/hal.h>
#include <sys/testing.h>

TEST_GROUP(pit_test);

static volatile uint32_t testVar;

static int callback(struct regs *r, void *p)
{
    testVar++;
    return 0;
}

TEST_SETUP(pit_test)
{
    testVar = 0;
    register_interrupt_handler(32, &callback, (void*)0xDEADBEEF);
}
TEST_TEAR_DOWN(pit_test)
{
    unregister_interrupt_handler(32, &callback, (void*)0xDEADBEEF);
}


TEST(pit_test, didTimePass)
{
    while(testVar < 200);
    TEST_ASSERT(testVar != 0); // Make sure time has passed
}

TEST(pit_test, updatedTimestamp)
{
    uint64_t ts = get_timestamp();
    while(testVar < 200);
    TEST_ASSERT(get_timestamp() > ts);
}
