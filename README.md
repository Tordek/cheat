CHEAT - The sillily-named C testing framework
=============================================

CHEAT is a minimal testing framework which tries to avoid making you write
anything more than your tests.

To use it, you simply `#include "cheat.h"`. If you're not using GCC, you may
need this guard:

    #ifndef __BASE_FILE__
    #define __BASE_FILE__ __FILE__
    #endif

Tests are defined with the `TEST` macro. You may also use the `GLOBALS`,
`SETUP` and `TEAR_DOWN` macros to centralize your initializations.

---

CHEAT is Copyrighted (c) 2012 Guillermo "Tordek" Freschi.
CHEAT is provided under a 2-clause BSD license.
