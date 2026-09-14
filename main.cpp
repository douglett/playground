#include <iostream>
#include "src/globals.hpp"
using namespace std;

// define globals
GFX gfx;

int main() {
	printf("starting Playground...\n");
	gfx.screen.width = 800;
	gfx.screen.height = 600;
	gfx.screen.zoom = 1;
	// SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	gfx.init();
	SetWindowState(FLAG_WINDOW_RESIZABLE);

	while (!gfx.shouldQuit()) {
		gfx.flip();
	}

	gfx.destroy();
}
