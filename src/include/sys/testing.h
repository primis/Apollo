/*
 * (c) 2018 Apollo Developers
 * testing.h - integrate tests tighter into kernel.
 */

#ifndef __SYS_TESTING_H
#define __SYS_TESTING_H

#include <unity_fixture.h>

typedef struct test_mod {
    void (*runner)(void);
} test_mod_t;

#undef TEST

#define TEST(group, name)                                   \
    void TEST_##group##_##name##_(void);                    \
    void TEST_##group##_##name##_run(void);                 \
    static test_mod_t run_##group##_##name##_test           \
    __attribute__((__section__("testing"),used)) = {        \
        .runner = &TEST_##group##_##name##_run };           \
    void TEST_##group##_##name##_run(void)                  \
    {\
        UnityTestRunner(TEST_##group##_SETUP,\
            TEST_##group##_##name##_,\
            TEST_##group##_TEAR_DOWN,\
            "TEST(" #group ", " #name ")",\
            TEST_GROUP_##group, #name,\
            __FILE__, __LINE__);\
    }\
    void  TEST_##group##_##name##_(void)

#endif
