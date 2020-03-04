# gba-coroutines
Provides basic coroutines for gba homebrew.

The example is compiled with -O3 and -flto.

The Makefile is based on the one from devkitARM.

# Stack size
You can use -fstack-usage to find the stack usage of your functions.
coYield uses 36 bytes to store the state of the coroutine.
Keep in mind that GCC can not find the stack usage of assembly code.

In the example -fstack-usage reports that waitFrames uses 24 bytes,
coroutineFunc1 use 24 and coroutine2 16. Thus coroutine1 needs 24+24+36 bytes of stack
plus 4 bytes to store the coroutine state, ie 88 bytes, or 22 words while
coroutine2 only needs 20 words.

# Safety
There is no safety, expect weirdness if you
* Run out of stack
* Call coSelfInitCoroutine inside a coroutine
* Call coRunCoroutine inside a coroutine
* Call coYield when not in a coroutine

If things crash or behave strangely you might be running out of stack space.

# Dependencies
The library should have no dependencies apart from a GCC, but it has only been tested with devkitARM.
The example also depends on libgba.