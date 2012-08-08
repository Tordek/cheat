#ifndef _cheat_h_
#define _cheat_h_

#include <stdlib.h>
#include <stdio.h>

typedef void cheat_test();

/* First pass: Function declarations. */

#define TEST(test_name, test_body) static void test_##test_name();
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

int main(int argc, char *argv[])
{
    cheat_test *tests[] = {
#include __BASE_FILE__
        NULL
    };

    cheat_test **current_test = tests;

    while (*current_test) {
        cheat_set_up();
        (*current_test)();
        cheat_tear_down();

        current_test++;
    }

    return 0;
}

#undef TEST
#undef SET_UP
#undef TEAR_DOWN
#undef GLOBALS

/* Third pass: Function definitions */

#define TEST(test_name, test_body) static void test_##test_name() test_body
#define SET_UP(body) static void cheat_set_up() body
#define TEAR_DOWN(body) static void cheat_tear_down() body
#define GLOBALS(body) body

#endif
