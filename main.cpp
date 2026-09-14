#include <iostream>
#include "src/globals.hpp"
using namespace std;

// define globals
GFX gfx;

int main() {
	printf("starting Playground...\n");
	gfx.screen.width = 400;
	gfx.screen.height = 300;
	gfx.screen.zoom = 2;
	// gfx.qbfont.selectedfont = 1;
	// SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	gfx.init();
	SetWindowState(FLAG_WINDOW_RESIZABLE);

	while (!gfx.shouldQuit()) {
		DrawTexture(gfx.qbfont.qb13.texture, 10, 100, WHITE);
		gfx.print("Hello World!", 10, 10, BLUE);
		gfx.flip();
	}

	gfx.destroy();
}
