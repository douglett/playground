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
	// SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	gfx.init();
	SetWindowState(FLAG_WINDOW_RESIZABLE);

	while (!gfx.shouldQuit()) {
		auto& qb1 = gfx.qbfont.qb13;
		DrawTexture(qb1.texture, 10, 10, WHITE);
		gfx.flip();
	}

	gfx.destroy();
}
