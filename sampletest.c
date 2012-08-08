/* The biggest hack we use is a GCC macro.
 * This optional define should be enough to make this work on most compilers.
 */

#ifndef __BASE_FILE__
#define __BASE_FILE__ __FILE__
#endif

#include "cheat.h"
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
