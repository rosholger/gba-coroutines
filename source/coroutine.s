    .file "coroutine.s"
    .extern _coMainCoroutineStackPointer
    .extern _coCurrentCoroutineState

    .macro pushRegs
    push {r4-r12, lr}
    .endm

    .macro popRegs
    pop {r4-r12, lr}
    .endm

    .section .iwram, "ax", %progbits
    .arm
    .align 2
    .global coInitCoroutineEx
    .global coRunCoroutine
    .global coYield
    .global coSelfInitCoroutineEx
    // layout of the coroutine object is
    // * stack pointer
    // * stack
    // r0 is the function pointer
    // r1 is a pointer to the coroutine object
    // r2 is the size of the coroutine object
coInitCoroutineEx:
    push {lr}
    mov lr, r0              // lr is now the corountine function
    sub r2, #4
    add r2, r1              // r2 is now a pointer to the end of the coroutine object aka its stack
    mov r3, r13             // store sp in r2 for safe keeping
    mov r13, r2             // we now use coroutine stack
    pushRegs                // bump sp and store the coroutine function in lr
    str r13, [r1]           // store coroutine sp at start of coroutine object
    mov r13, r3             // we are back to normal sp
    pop {lr}
    bx lr                   // return
    // r0 is a pointer to the coroutine object
    // returns (from the callers point of view) false if the coroutine finished, otherwise true
coRunCoroutine:
    ldr r2, =_coCurrentCoroutineState
    str r0, [r2]            // store the pointer to the coroutine object in _coCurrentCoroutineState
    pushRegs                // push all non-volatile registers except the stack pointer
    ldr r2, =_coMainCoroutineStackPointer
    str r13, [r2]           // store the main stack pointer
    ldr r13, [r0]           // we now use the coroutine stack!!
    popRegs                 // restore coroutine registers
    mov r0, lr              // free up lr
    ldr lr, =_coCleanupCoroutine // set lr, this is discarded if its after a yield, since it will assume that lr was used to return
    bx r0                   // return to start of coroutine function/from yield
coYield:
    ldr r1, =_coCurrentCoroutineState  // load pointer to pointer to coroutine object
    ldr r1, [r1]                    // strip away one pointer layer
    pushRegs                        // push all non-volatile registers except the stack pointer
    str r13, [r1]                   // store coroutine sp at start of coroutine object
    ldr r2, =_coMainCoroutineStackPointer // get the pointer to the memory where the main sp is stored
    ldr r13, [r2]                   // we now use main stack!!
    popRegs                         // restore main registers
    mov r0, #1                      // The coroutine did not finish
    bx lr                           // return to after runCoroutine
_coCleanupCoroutine:
    ldr r2, =_coMainCoroutineStackPointer // get the pointer to the memory where the main sp is stored
    ldr r13, [r2]                   // we now use main stack!!
    popRegs                         // restore main registers
    mov r0, #0                      // The coroutine finished
    bx lr                           // return to after runCoroutine
    // r0 is the function pointer
    // r1 is a pointer to the coroutine object
    // r2 is the size of the coroutine object
    // r3 is a data pointer used by the coroutine to initialize
coSelfInitCoroutineEx:
    sub r2, #4
    add r2, r1              // r2 is now a pointer to the end of the coroutine object aka its stack
    str r2, [r1]            // store coroutine stack pointer at in coroutine object
    pushRegs                // store main registers, r4-r12 is free
    ldr r4, =_coMainCoroutineStackPointer
    str r13, [r4]           // store main stack pointer
    mov r13, r2             // we are now using coroutine stack, r2 is now free
    ldr r2, =_coCurrentCoroutineState
    str r1, [r2]            // store the pointer to the coroutine object in _coCurrentCoroutineState, r2 is again free
    mov r1, r0              // shuffle regs to free up r0
    mov r0, r3              // move data pointer to r0
    ldr lr, =_coCleanupCoroutine
    bx r1

    .bss
_coMainCoroutineStackPointer:
    .space 4
_coCurrentCoroutineState:
    .space 4