#pragma once
#include "raylib.h"
#include "screen.hpp"
#include "qbfont.hpp"
#include "container.hpp"
#include "shape.hpp"
#include "sprite.hpp"
#include "tilemap.hpp"

struct GFX {
	struct rect { int x, y, w, h; };

	static inline Screen screen;
	static inline QBFont font;

	// forward to screen
	static int  init()    { return screen.init() || font.init(); }
	static void destroy() { screen.destroy(); }
	static void begin()   { screen.begin(); }
	static void flip()    { screen.flip(); }

	static void text(const string& str, int x, int y, Color col=WHITE) {
		screen.text(str, x, y, col);
	}

	static void blitt(Texture2D texture, int tsize, int tile, int x, int y, Color blend=WHITE) {
		screen.blitt(texture, tsize, tile, x, y, blend);
	}

	static void blittr(Texture2D texture, int tsize, int tile, int x, int y, float rot, Color blend=WHITE) {
		screen.blittr(texture, tsize, tile, x, y, rot, blend);
	}

	// forward to qbfont
	static void print(const string& str, int x, int y, Color col=WHITE) {
		font.print(str, x, y, col);
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
