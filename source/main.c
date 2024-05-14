#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <math.h>


// n3dsxl uses portrait screens
// #define SCR_TOP_W 240
// #define SCR_TOP_H 400
#define SCR_BOTTOM_W 240
#define SCR_BOTTOM_H 320

uint32_t getTouchFBValue(touchPosition* pos) {
	touchPosition clamped = *pos;
	if (clamped.px > SCR_BOTTOM_H)
		clamped.px = SCR_BOTTOM_H;
	if (clamped.py > SCR_BOTTOM_W)
		clamped.px = SCR_BOTTOM_W;

	int res = (-4 + ((clamped.px * SCR_BOTTOM_W) + abs(clamped.py - SCR_BOTTOM_W)) * 4);

	if (res > 0) {
		return res; 
	} else {
		return 0;
	}
}

void dirtyWriteToFB(u8* fb, int* addr, u8 to_write) {
	fb[*addr] = to_write; 
	fb[*addr + 1] = to_write; 
	fb[*addr + 2] = to_write; 
	fb[*addr + 3] = to_write; 
}

void printInfo(int* x, touchPosition* t) {
	printf("fb[%d], touch.x = %d, touch.y = %d\n", *x, t->px, t->py);
}


int main(int argc, char* argv[])
{
	// init with rgba8 colors (4 byte)
    gfxInit(GSP_RGBA8_OES, GSP_RGBA8_OES, false);

	// console for debugging
	PrintConsole* top_console = consoleInit(GFX_TOP, NULL);
	consoleSelect(top_console);

	// main loop
	while (aptMainLoop())
	{
		gspWaitForVBlank();
		hidScanInput();

		u8* fb_bottom = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);

		// clear screen
		memset(fb_bottom, 0x80, SCR_BOTTOM_H * SCR_BOTTOM_W * 4);

		// px314, py234 MAX
		touchPosition touch = {};
		hidTouchRead(&touch);
		int x = getTouchFBValue(&touch);
		dirtyWriteToFB(fb_bottom, &x, 0xff);

		// print touch values to top console
		printInfo(&x, &touch);

		gfxSwapBuffers();
		gfxFlushBuffers();

		// press start to exit
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; 
	}

	gfxExit();
	return 0;
}
