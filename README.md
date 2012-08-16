[![Build Status](https://secure.travis-ci.org/Tordek/cheat.png)](http://travis-ci.org/Tordek/cheat)

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
`SET_UP` and `TEAR_DOWN` macros to centralize your initializations.

There are additional helpers in the `cheat_helpers.h` file, which provide
stuff to help you test some harder to test stuff.

Running
-------

When you compile your test file, you get a complete executable. It shows a
pretty progress bar thingie and all.

By default, it runs each test in an isolated subprocess (assuming your system
is supported (and that currently includes only Unix (and has only been tested
in Linux)), so all tests still run, even if one segfaults. You can specify the
`--nofork` flag, to make it run everything in the same process, if you need to.

---

CHEAT is Copyrighted (c) 2012 Guillermo "Tordek" Freschi.
CHEAT is provided under a 2-clause BSD license.
