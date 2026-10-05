#pragma once
#include "raylib.h"
#include "qbfont.hpp"

struct GFX {
	struct rect { int x, y, w, h; };
	// submodules
	static inline QBFont font;
	// vars
	static inline string winname;
	static inline Color bgcolor = BLACK;
	static inline int flag_fps = 1;

	// -- Screen Management --
	static int init(int width=800, int height=600, const string& name="GFX:Game") {
		SetTraceLogLevel(LOG_WARNING);
		InitWindow(width, height, winname.c_str());
		if (!IsWindowReady())  return 1;
		SetTargetFPS(60);
		// submodules
		font.init();
		// ok
		printf("Screen initialized: %d %d\n", width, height);
		begin();
		return 0;
	}
	static void destroy() {
		font.destroy();
		CloseWindow();  // Close window and OpenGL context
	}
	static void begin()   {
		BeginDrawing();
		ClearBackground(bgcolor);
	}
	static void flip() {
		if (flag_fps) {
			string s = to_string(GetFPS());
			print(s, GetScreenWidth()-font.width(s)-2, 2, GREEN);
		}
		EndDrawing();  // flip
		begin();  // begin drawing mode for next frame
	}

	// -- Basic Drawing --
	static void print(const string& str, int x, int y, Color col=WHITE) {
		font.print(str, x, y, col);
	}
	static void text(const string& str, int x, int y, Color col=WHITE) {
		DrawText(str.c_str(), x, y, 10, col);
	}
	static void blitt(Texture2D texture, int tsize, int tile, int x, int y, Color blend=WHITE) {
		int tx = tile % (texture.width / tsize);
		int ty = tile / (texture.width / tsize);
		float t = tsize;
		Rectangle src{ tx*t, ty*t, t, t };
		Vector2   dst{ float(x), float(y) };
		DrawTextureRec(texture, src, dst, blend);
	}
	static void blittr(Texture2D texture, int tsize, int tile, int x, int y, float rot, Color blend=WHITE) {
		int tx = tile % (texture.width / tsize);
		int ty = tile / (texture.width / tsize);
		float t = tsize;
		Rectangle src{ tx*t, ty*t, t, t };
		Vector2   ori{ float(tsize) / 2, float(tsize) / 2 };
		Rectangle dst{ x+ori.x, y+ori.y, float(tsize), float(tsize) };
		DrawTexturePro(texture, src, dst, ori, rot, blend);
	}

	// helpers
	static int  screenw()    { return GetScreenWidth(); }
	static int  screenh()    { return GetScreenHeight(); }
	static bool shouldquit() { return WindowShouldClose(); }
	static void fullscreen() { togglestate(FLAG_BORDERLESS_WINDOWED_MODE); }
	static void resizable()  { togglestate(FLAG_WINDOW_RESIZABLE); }
	static void togglestate(ConfigFlags flag) { IsWindowState(flag) ? ClearWindowState(flag) : SetWindowState(flag); }
	static float dir2rot(int dir) { return 360.0 / 4 * dir; }
	static rect dir2point(int dir, int d=1) {
		switch (dir) {
			case 0:   return {  0, -d };
			case 1:   return {  d,  0 };
			case 2:   return {  0,  d };
			case 3:   return { -d,  0 };
			default:  return {  0,  0 };
		}
	}
};
