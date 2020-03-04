#ifndef COROUTINE_H
#define COROUTINE_H 1

// Initializes coroutineObject such that it can be used with runCoroutine.
// Does not run the coroutine, use this for coroutines that do not take any arguments.
// coroutineObject MUST be aligned to 4 bytes and MUST be able to fit the stack
// of the coroutine + 4 bytes.
__attribute__((target("arm"), section(".iwram"), long_call))
extern void coInitCoroutineEx(void (*coroutineFunction)(),
                              int *coroutineObject,
                              unsigned int coroutineObjectSizeInBytes);

// Initializes coroutineObject similarly to coInitCoroutineEx,
// but the coroutine function takes a void * and also runs the
// coroutine until the first coYield or until it finishes.
// returns 1 if the coroutine finished, otherwise false.
// Use this for coroutines that wants to be supplied arguments.
// DO NOT call this from within a coroutine!
// Same requirements on coroutineIbject as coInitCoroutineEx.
__attribute__((target("arm"), section(".iwram"), long_call))
extern unsigned int coSelfInitCoroutineEx(void (*coroutineFunction)(void *),
                                          int *coroutineObject,
                                          unsigned int coroutineObjectSizeInBytes,
                                          void *initData);

// Resumes an initialized coroutine until the next coYield.
// returns 1 if the coroutine finished, otherwise false.
// DO NOT call this with a coroutineObject that has finished!
// DO NOT call this from within a coroutine!
__attribute__((target("arm"), section(".iwram"), long_call))
extern unsigned int coRunCoroutine(int *coroutineObject);

// Pause execution of a coroutine and return from coSelfInitCoroutineEx/coRunCoroutine.
// ONLY call this from within a coroutine.
__attribute__((target("arm"), section(".iwram"), long_call))
extern void coYield();


// Convenience macro for when coroutineObject is an array, NOT a pointer
#define coInitCoroutine(fp, coroutineObject)                            \
    coInitCoroutineEx((fp), (coroutineObject), sizeof(coroutineObject))

// Convenience macro for when coroutineObject is an array, NOT a pointer
#define coSelfInitCoroutine(fp, coroutineObject, data)                  \
    coSelfInitCoroutineEx((fp), (coroutineObject), sizeof(coroutineObject), (data))

#endif