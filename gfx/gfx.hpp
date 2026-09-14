#pragma once
#include "raylib.h"
#include "screen.hpp"
#include "container.hpp"
#include "shape.hpp"
#include "sprite.hpp"
#include "tilemap.hpp"

struct GFX {
	struct rect { int x, y, w, h; };

	Screen screen;

	// forward to screen
	int  init()    { return screen.init(); }
	void destroy() { screen.destroy(); }
	void begin()   { screen.begin(); }
	void flip()    { screen.flip(); }

	void text(const string& str, int x, int y, Color col=WHITE) {
		screen.text(str, x, y, col);
	}

	void blitt(Texture2D texture, int tsize, int tile, int x, int y, Color blend=WHITE) {
		screen.blitt(texture, tsize, tile, x, y, blend);
	}

	void blittr(Texture2D texture, int tsize, int tile, int x, int y, float rot, Color blend=WHITE) {
		screen.blittr(texture, tsize, tile, x, y, rot, blend);
	}

	// helpers
	static bool shouldQuit() { return WindowShouldClose(); }

	static rect dir2point(int dir, int d=1) {
		switch (dir) {
			case 0:   return {  0, -d };
			case 1:   return {  d,  0 };
			case 2:   return {  0,  d };
			case 3:   return { -d,  0 };
			default:  return {  0,  0 };
		}
	}

	static float dir2rot(int dir) {
		return 360.0 / 4 * dir;
	}
};
