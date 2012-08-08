#ifndef _cheat_h_
#define _cheat_h_

#include <stdlib.h>
#include <stdio.h>

typedef void cheat_test();

struct cheat_test_suite {
    int test_count;
    int test_failures;
    int last_test_succeeded;
};

/* First pass: Function declarations. */

#define TEST(test_name, test_body) static void test_##test_name(struct cheat_test_suite *suite);
#define SET_UP(body) static void cheat_set_up();
#define TEAR_DOWN(body) static void cheat_tear_down();
#define GLOBALS(body)

#include __BASE_FILE__

#undef TEST
#undef SET_UP
#undef TEAR_DOWN
#undef GLOBALS

/* Second pass: Listing Functions */

#define TEST(test_name, body) test_##test_name,
#define SET_UP(body)
#define TEAR_DOWN(body)
#define GLOBALS(body)

static void cheat_suite_init(struct cheat_test_suite *suite)
{
    suite->test_count = 0;
    suite->test_failures = 0;
}

static void cheat_suite_summary(struct cheat_test_suite *suite)
{
    printf("\n\n%d failed tests of %d tests run.\n", suite->test_failures, suite->test_count);
}

static void cheat_test_prepare(struct cheat_test_suite *suite)
{
    suite->last_test_succeeded = 1;
}

static void cheat_test_end(struct cheat_test_suite *suite)
{
    suite->test_count++;
    if (!suite->last_test_succeeded) {
        suite->test_failures++;
        printf("F");
    } else {
        printf(".");
    }
}

int main(int argc, char *argv[])
{
    struct cheat_test_suite suite;

    cheat_test *tests[] = {
#include __BASE_FILE__
        NULL
    };

    cheat_test **current_test = tests;

    cheat_suite_init(&suite);

    while (*current_test) {
        cheat_test_prepare(&suite);

        cheat_set_up();
        (*current_test)(&suite);
        cheat_tear_down();

        cheat_test_end(&suite);

        current_test++;
    }

    cheat_suite_summary(&suite);

    return 0;
}

#undef TEST
#undef SET_UP
#undef TEAR_DOWN
#undef GLOBALS

/* Third pass: Function definitions */

#define TEST(test_name, test_body) static void test_##test_name(struct cheat_test_suite *suite) test_body
#define SET_UP(body) static void cheat_set_up() body
#define TEAR_DOWN(body) static void cheat_tear_down() body
#define GLOBALS(body) body

#define cheat_assert(assertion) if (!assertion) { suite->last_test_succeeded = 0; }

#endif
