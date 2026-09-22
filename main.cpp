#include <iostream>
#include "src/globals.hpp"
using namespace std;

// define globals
GFX gfx;

void painttext() {
	auto& f = gfx.qbfont.getselectedfont();
	int tw = gfx.screen.width / f.charw;
	int th = gfx.screen.height / f.charh;
	// gfx.print(to_string(tw) + " " + to_string(th), 10, 10);

	for (int y = 0; y < th; y++)
	for (int x = 0; x < tw; x++) {
		auto s = x == 0 || y == 0 || x == tw-1 || y == th-1 ? "@" : " ";
		gfx.print(s, x*f.charw, y*f.charh);
	}
}

int main() {
	printf("starting Playground...\n");
	// setup window
	gfx.screen.width = 400;
	gfx.screen.height = 300;
	gfx.screen.zoom = 2;
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	gfx.init();
	// SetWindowState(FLAG_WINDOW_RESIZABLE);

	while (!gfx.shouldQuit()) {
		if ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && IsKeyPressed(KEY_ENTER))
			gfx.screen.fullscreen();

		// auto z = gfx.screen.zoom;
		// DrawRectangle(10, 10, (GetScreenWidth()/z)-20, (GetScreenHeight()/z)-20, Color{64, 0, 255, 128});

		// DrawTexture(gfx.qbfont.qb13.texture, 10, 100, WHITE);
		// gfx.print("Hello World!", 10, 10, BLUE);

		painttext();
		gfx.flip();
	}

	gfx.destroy();
}
