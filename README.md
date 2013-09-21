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

By default, it runs each test in an isolated subprocess (tested, so far, in
Windows and Linux), so all tests still run, even if one segfaults. You can
specify the `--nofork` flag, to make it run everything in the same process, if
you need to.


Why?
----

Because abusing systems for hackish results is fun. This was an exercise in
how far I could push the C preprocessor to build a "framework" with a simple
syntax for the user.

It's not meant for serious work, and it's easy to run into issues (do report
them and send patches!), but if it works for you, more power to you!

Known Issues (don't say I didn't warn you)
------------------------------------------

If `cheat.h` is placed in a global include directory (like, say `/usr/include`),
and `__BASE_FILE__` is a relative path, CHEAT won't work. Put `cheat.h` in your
project directory.

---

CHEAT is Copyrighted (c) 2012 Guillermo "Tordek" Freschi.

CHEAT is provided under a 2-clause BSD license.
