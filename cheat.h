#ifndef _cheat_h_
#define _cheat_h_

#ifndef __BASE_FILE__
#error "The __BASE_FILE__ macro is not defined. Check the README for help."
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

enum cheat_test_status {
    CHEAT_SUCCESS,
    CHEAT_FAILURE,
    CHEAT_IGNORE
};

struct cheat_test_suite {
    int test_count;
    int test_failures;
    int last_test_status;
    char **log;
    size_t log_size;
    int stdout_fd;
};

typedef void cheat_test(struct cheat_test_suite *suite);

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
    suite->log = NULL;
    suite->log_size = 0;
    suite->stdout_fd = dup(STDOUT_FILENO);
    char name_template[] = "cheat_test_XXXXXX";
    int fakestdout = mkstemp(name_template);
    dup2(fakestdout, STDOUT_FILENO);
    setbuf(stdout, NULL);
}

static void cheat_suite_cleanup(struct cheat_test_suite *suite)
{
    dup2(suite->stdout_fd, STDOUT_FILENO);
}

static void cheat_suite_summary(struct cheat_test_suite *suite)
{
    if (suite->log) {
        int i;

        printf("\n");
        for (i = 0; i < suite->log_size; ++i) {
            printf("%s", suite->log[i]);
        }

        free(suite->log);
    }

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
            write(suite->stdout_fd, ".", 1);
            break;
        case CHEAT_FAILURE:
            write(suite->stdout_fd, "F", 1);
            suite->test_failures++;
            break;
        case CHEAT_IGNORE:
            write(suite->stdout_fd, "I", 1);
            break;
    }
}

static void cheat_log_append(struct cheat_test_suite *suite, char *message)
{
    suite->log_size++;
    suite->log = realloc(suite->log, (suite->log_size + 1) * sizeof(char *));
    suite->log[suite->log_size - 1] = message;
}

static void cheat_test_assert(struct cheat_test_suite *suite, int result, char *message)
{
    if (result != 0)
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

    cheat_suite_cleanup(&suite);

    cheat_suite_summary(&suite);

    return suite.test_failures;
}

#undef TEST
#undef SET_UP
#undef TEAR_DOWN
#undef GLOBALS

/* Third pass: Function definitions */
/* Also, public interface. You're only suppossed to use stuff below this line.
 */

#define TEST(test_name, test_body) static void test_##test_name(struct cheat_test_suite *suite) test_body
#define SET_UP(body) static void cheat_set_up() body
#define TEAR_DOWN(body) static void cheat_tear_down() body
#define GLOBALS(body) body

#define cheat_assert(assertion) cheat_test_assert(suite, assertion, __FILE__ " failed assertion: " #assertion "\n")

int cheat_output_contains(char *contents)
{
    lseek(STDOUT_FILENO, 0, SEEK_SET);
    char buffer[255];
    read(STDOUT_FILENO, buffer, 255);
    return strstr(buffer, contents) != NULL;
}

#endif
