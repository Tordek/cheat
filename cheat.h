#ifndef CHEAT_H
#define CHEAT_H

#ifndef __BASE_FILE__
#error "The __BASE_FILE__ macro is not defined. Check the README for help."
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

enum cheat_test_status {
    CHEAT_SUCCESS,
    CHEAT_FAILURE,
    CHEAT_IGNORE,
    CHEAT_SEGFAULT
};

struct cheat_test_suite {
    int test_count;
    int test_failures;
    enum cheat_test_status last_test_status;
    char **log;
    char *argv0;
    size_t log_size;
    int nofork;
    FILE *stdout;
};

typedef void cheat_test(struct cheat_test_suite *suite);

struct cheat_test_s {
    char *name;
    cheat_test *test;
};

/* Public interface: helpers */

#define TEST_IGNORE(test_name, test_body) TEST(test_name, { suite->last_test_status = CHEAT_IGNORE; })

/* First pass: Function declarations. */

#define TEST(test_name, test_body) static void test_##test_name(struct cheat_test_suite *suite);
#define SET_UP(body) static void cheat_set_up(void);
#define TEAR_DOWN(body) static void cheat_tear_down(void);
#define GLOBALS(body)

#include __BASE_FILE__

#undef TEST
#undef SET_UP
#undef TEAR_DOWN
#undef GLOBALS

static void cheat_suite_init(struct cheat_test_suite *suite, char *argv0)
{
    memset(suite, 0, sizeof(struct cheat_test_suite));
    suite->stdout = stdout;
    suite->argv0 = argv0;
}

static void cheat_suite_summary(struct cheat_test_suite *suite)
{
    if (suite->log) {
        size_t i;

        fputs("\n", suite->stdout);
        for (i = 0; i < suite->log_size; ++i) {
            fputs(suite->log[i], suite->stdout);
            free(suite->log[i]);
        }

        free(suite->log);
    }

    fprintf(suite->stdout, "\n%d failed tests of %d tests run.\n", suite->test_failures, suite->test_count);
}

static void cheat_test_end(struct cheat_test_suite *suite)
{
    suite->test_count++;

    switch (suite->last_test_status) {
        case CHEAT_SUCCESS:
            fputc('.', suite->stdout);
            break;
        case CHEAT_FAILURE:
            fputc('F', suite->stdout);
            suite->test_failures++;
            break;
        case CHEAT_IGNORE:
            fputc('I', suite->stdout);
            break;
        case CHEAT_SEGFAULT:
            fputc('S', suite->stdout);
            suite->test_failures++;
            break;
        default:
            exit(-1);
    }
}

static void cheat_log_append(struct cheat_test_suite *suite, char *message, int len)
{
    char *buf = malloc(len);
    memcpy(buf, message, len);

    suite->log_size++;
    suite->log = realloc(suite->log, (suite->log_size + 1) * sizeof(char *));
    suite->log[suite->log_size - 1] = buf; /* We give up our buffer! */
}

static void cheat_test_assert(
        struct cheat_test_suite *suite,
        int result,
        char *assertion,
        char *filename,
        int line)
{
    if (result != 0)
        return;

    suite->last_test_status = CHEAT_FAILURE;
    if (suite->nofork) {
        char *buffer = NULL;
        int len = 255;
        int bufsize;

        do {
            bufsize = len;
            buffer = realloc(buffer, bufsize);
            len = snprintf(buffer, bufsize,
                    "%s:%d: Assertion failed: '%s'.\n",
                    filename,
                    line,
                    assertion);
        } while (bufsize != len);

        cheat_log_append(suite, buffer, bufsize);
    } else {
        fprintf(suite->stdout,
                "%s:%d: Assertion failed: '%s'.\n",
                filename,
                line,
                assertion);
    }
}

static int run_test(struct cheat_test_s *test, struct cheat_test_suite *suite)
{
    suite->last_test_status = CHEAT_SUCCESS;

    cheat_set_up();
    (test->test)(suite);
    cheat_tear_down();

    return suite->last_test_status;
}

#ifdef unix
#include <sys/types.h>
#include <sys/wait.h>
#endif

static void run_isolated_test(struct cheat_test_s *test, struct cheat_test_suite *suite)
{
#ifdef unix
    pid_t pid;
    int pipefd[2];

    pipe(pipefd);
    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        execl(suite->argv0, suite->argv0, test->name, NULL);
        exit(EXIT_FAILURE);
    } else {
        int status;
        char buf[255];
        int len;

        close(pipefd[1]);
        while ((len = read(pipefd[0], buf, 255)) != 0) {
            buf[len] = 0;
            cheat_log_append(suite, buf, len + 1);
        }

        waitpid(pid, &status, 0);
        close(pipefd[0]);
        suite->last_test_status = WIFEXITED(status) ? WEXITSTATUS(status)
                                                    : CHEAT_SEGFAULT;
    }
#else
    fputs("Running isolated tests not supported in this environment. Please use --nofork.\n", stderr);
#endif
}

/* Second pass: Listing Functions */

#define TEST(test_name, body) { #test_name, test_##test_name },
#define SET_UP(body)
#define TEAR_DOWN(body)
#define GLOBALS(body)

int main(int argc, char *argv[])
{
    struct cheat_test_suite suite;

    struct cheat_test_s tests[] = {
#include __BASE_FILE__
    };

    int i;
    const int test_count = sizeof(tests) / sizeof (struct cheat_test_s);

    cheat_suite_init(&suite, argv[0]);

    suite.nofork = 0;

    if (argc > 1) {
        if (argv[1][0] == '-') {
            if (strcmp(argv[1], "--nofork") == 0) {
                suite.nofork = 1;
            }
        } else {
            for (i = 0; i < test_count; ++i) {
                struct cheat_test_s current_test = tests[i];

                if (strcmp(argv[1], current_test.name) == 0) {
                    return run_test(&current_test, &suite);
                }
            }

            return -1;
        }
    }

    for (i = 0; i < test_count; ++i) {
        struct cheat_test_s current_test = tests[i];

        if (suite.nofork) {
            run_test(&current_test, &suite);
        } else {
            run_isolated_test(&current_test, &suite);
        }

        cheat_test_end(&suite);
    }

    cheat_suite_summary(&suite);

    return suite.test_failures;
}

#undef TEST
#undef SET_UP
#undef TEAR_DOWN
#undef GLOBALS

/* Third pass: Function definitions */
/* Part of the public interface. See at the top for more helpers. */

#define TEST(test_name, test_body) static void test_##test_name(struct cheat_test_suite *suite) test_body
#define SET_UP(body) static void cheat_set_up(void) body
#define TEAR_DOWN(body) static void cheat_tear_down(void) body
#define GLOBALS(body) body

#define cheat_assert(assertion) cheat_test_assert(suite, assertion, #assertion, __FILE__, __LINE__)

#endif
