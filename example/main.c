
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>
#include "coroutine.h"

int coroutine1[22];
int coroutine2[20];

void waitFrames(unsigned int frames) {
    for (unsigned int i = 0; i < frames; ++i) {
        coYield();
    }
}

const char *displayedText;

void coroutineFunc1(void *data) {
    unsigned int numberOfFrames = *(unsigned int *)data;
    coYield();
	for(unsigned int i = 0; i < 2; ++i) {
        displayedText = "\x1b[10;10HCoroutine1 1!\n";
        waitFrames(numberOfFrames);
        displayedText = "\x1b[10;10HCoroutine1 2!\n";
        waitFrames(numberOfFrames);
	}
}

void coroutineFunc2() {
	for(unsigned int i = 0; i < 3; ++i) {
        displayedText = "\x1b[10;10HCoroutine2 1!\n";
        waitFrames(30);
        displayedText = "\x1b[10;10HCoroutine2 2!\n";
        waitFrames(30);
	}
}

//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------


	// the vblank interrupt must be enabled for VBlankIntrWait() to work
	// since the default dispatcher handles the bios flags no vblank handler
	// is required
	irqInit();
	irqEnable(IRQ_VBLANK);

    consoleDemoInit();
    int frames = 120;
    coSelfInitCoroutine(coroutineFunc1, coroutine1, &frames);

    while (coRunCoroutine(coroutine1)) {
		VBlankIntrWait();
        iprintf(displayedText);
    }

    coInitCoroutine(coroutineFunc2, coroutine2);

    while (coRunCoroutine(coroutine2)) {
        VBlankIntrWait();
        iprintf(displayedText);
    }

    iprintf("\x1b[10;10HCoroutines Finished!\n");

	while (1) {
		VBlankIntrWait();
	}
}
