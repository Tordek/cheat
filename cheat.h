#ifndef _cheat_h_
#define _cheat_h_

#include <stdlib.h>
#include <stdio.h>

#define CHEAT_LOG_SIZE 256

typedef void cheat_test();

enum cheat_test_status {
    CHEAT_SUCCESS,
    CHEAT_FAILURE,
    CHEAT_IGNORE
};

struct cheat_test_suite {
    int test_count;
    int test_failures;
    int last_test_status;
    char *log;
    size_t log_len;
    size_t log_size;
};

/* First pass: Function declarations. */

#define TEST(test_name, test_body) static void test_##test_name(struct cheat_test_suite *suite);
#define TEST_IGNORE(test_name, test_body) TEST(test_name, { suite->last_test_status = CHEAT_IGNORE; })
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
    suite->log = malloc(CHEAT_LOG_SIZE);
    suite->log_len = 0;
    suite->log_size = CHEAT_LOG_SIZE;
}

static void cheat_suite_summary(struct cheat_test_suite *suite)
{
    printf("\n%s", suite->log);
    printf("\n%d failed tests of %d tests run.\n", suite->test_failures, suite->test_count);
}

static void cheat_test_prepare(struct cheat_test_suite *suite)
{
    suite->last_test_status = CHEAT_SUCCESS;
}

static void cheat_test_end(struct cheat_test_suite *suite)
{
    suite->test_count++;
    switch (suite->last_test_status) {
        case CHEAT_SUCCESS:
            printf(".");
            break;
        case CHEAT_FAILURE:
            printf("F");
            suite->test_failures++;
            break;
        case CHEAT_IGNORE:
            printf("I");
            break;
    }
}

static void cheat_log_append(struct cheat_test_suite *suite, char *message)
{
    size_t message_len = strlen(message);

    while (suite->log_len + message_len > suite->log_size) {
        suite->log_size *= 2;
        suite->log = realloc(suite->log, suite->log_size);
    }

    strcat(suite->log, message);
}

static void cheat_test_assert(struct cheat_test_suite *suite, int result, char *message)
{
    if (result == 1)
        return;

    suite->last_test_status = CHEAT_FAILURE;
    cheat_log_append(suite, message);
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

#define cheat_assert(assertion) cheat_test_assert(suite, assertion, "Failed assertion on file " __FILE__ ": " #assertion "\n")

#endif
