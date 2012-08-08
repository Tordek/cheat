#ifndef _cheat_h_
#define _cheat_h_

#include <stdlib.h>
#include <stdio.h>

// First pass: Function declarations.

#define TEST(test_name, test_body) int test_##test_name();
#define SETUP(body) int setup();
#define TEAR_DOWN(body) int teardown();
#define GLOBALS(body)

#include __BASE_FILE__

#undef TEST
#undef SETUP
#undef TEAR_DOWN
#undef GLOBALS


// Second pass: Listing Functions

int main(int argc, char *argv[])
{
#define _cheat_main_

#define TEST(test_name, body) test_##test_name,
#define SETUP(body)
#define TEAR_DOWN(body)
#define GLOBALS(body)

    int (*tests[])(void *) = {
#include __BASE_FILE__
        NULL
    };

#undef TEST
#undef SETUP
#undef TEAR_DOWN
#undef GLOBALS

    int (*current_test)();
    int i;

    for (i = 0; tests[i] != NULL; ++i) {
        current_test = tests[i];
        current_test();
    }

    return 0;
#undef _cheat_main_
}

// Third pass: Function definitions

#define TEST(test_name, test_body) int test_##test_name() test_body
#define SETUP(body) int setup() body
#define TEAR_DOWN(body) int teardown() body
#define GLOBALS(body) body

#endif
