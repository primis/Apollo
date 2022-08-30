/*
 * (c) 2018 Apollo Developers
 * test_ringbuf.c - Testing the ringbuffer
 */

#include <adt/ringbuf.h>
#include <sys/testing.h>

static char_ringbuf_t test_buf;
static char buffer[9];
static char buff2[9];

TEST_GROUP(ringbuf_test);

TEST_SETUP(ringbuf_test)
{
    test_buf = make_char_ringbuf(buffer, 9);
}
TEST_TEAR_DOWN(ringbuf_test)
{

}

TEST(ringbuf_test, readWriteTest)
{
    char_ringbuf_write(&test_buf, "Hello", 6);
    char_ringbuf_read(&test_buf, buff2, 6);
    TEST_ASSERT_EQUAL_STRING("Hello", buff2);
}
