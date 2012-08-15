/* The biggest hack we use is a GCC macro.
 * This optional define should be enough to make this work on most compilers.
 */

#ifndef __BASE_FILE__
#define __BASE_FILE__ __FILE__
#endif

#include "cheat.h"
#include "cheat_helpers.h"
#include <string.h>

GLOBALS(
    char *tmp_string;
)

SET_UP({
    tmp_string = calloc(1, 50);
})

TEAR_DOWN({
    free(tmp_string);
})


TEST(maths_still_work, {
    cheat_assert(4 == 2+2);
})

TEST(strcat_makes_sense, {
    strcpy(tmp_string, "Hello, ");
    strcat(tmp_string, "World!");

    cheat_assert(0 == strcmp(tmp_string, "Hello, World!"));
})

TEST(failure, {
    cheat_assert(0);
})

TEST_IGNORE(ignored, {
        some_invalid.test->code();
})

TEST(second_failure, {
    cheat_assert(1 == 0);
})

TEST_WITH_CAPTURED_STDOUT(output_capture, {
    printf("Something stupid");
    cheat_assert(cheat_stream_contains(stdout, "Something"));
})

TEST_WITH_CAPTURED_STDOUT(large_output_capture, {
    printf("%1000s", "Potato");

    cheat_assert(cheat_stream_contains(stdout, "Potato"));
})

TEST_WITH_CAPTURED_STDERR(stderr_capture, {
    fprintf(stderr, "You can also capture errors!");
    cheat_assert(cheat_stream_contains(stderr, "errors"));
})

TEST(segfault, {
    int *foo = NULL;
    printf("%d", *foo);
    cheat_assert(0);
})
