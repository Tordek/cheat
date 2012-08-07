/* The biggest hack we use is a GCC macro.
 * This optional define should be enough to make this work on most compilers.
 */

#ifndef __BASE_FILE__
#define __BASE_FILE__ __FILE__
#endif

#include "ctf.h"
#include <assert.h>

GLOBALS(
    char *tmp_string;
)

SETUP({
    tmp_string = calloc(1, 50);
})

TEAR_DOWN({
    free(tmp_string);
})


TEST(maths_still_work, {
    assert(4 == 2+2);
})
