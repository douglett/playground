#include <iostream>
#include "src/globals.hpp"
using namespace std;

// define globals
GFX gfx;

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
		if ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && IsKeyPressed(KEY_ENTER)) {
			IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE)
				? ClearWindowState(FLAG_BORDERLESS_WINDOWED_MODE)
				: SetWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
		}

		auto z = gfx.screen.zoom;
		DrawRectangle(10, 10, (GetScreenWidth()/z)-20, (GetScreenHeight()/z)-20, Color{64, 0, 255, 128});

		DrawTexture(gfx.qbfont.qb13.texture, 10, 100, WHITE);
		gfx.print("Hello World!", 10, 10, BLUE);
		gfx.flip();
	}

	gfx.destroy();
}
