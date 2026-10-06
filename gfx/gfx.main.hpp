#pragma once
#include "raylib.h"
#include "qbfont.hpp"
#include "framebuffer.hpp"

struct GFX {
	struct rect { int x, y, w, h; };
	// submodules
	static inline QBFont font;
	static inline FrameBuffer buffer;
	static inline Assets assets;
	// vars
	static inline Color bgcolor = BLACK;
	static inline int flag_fps = 1;

	// -- Screen Management --
	static int init(int width=800, int height=600, const string& name="GFX:Game") {
		SetTraceLogLevel(LOG_WARNING);
		InitWindow(width, height, name.c_str());
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
		buffer.destroy();
		assets.destroy();
		CloseWindow();  // Close window and OpenGL context
	}
	static void usebuffer(int width, int height, FrameBuffer::SCALE_T scale=FrameBuffer::SCALE_STRETCH_FIT) {
		buffer.init(width, height, scale);
	}
	static void begin()   {
		BeginDrawing();
		ClearBackground(bgcolor);
		if (buffer.valid())
			BeginTextureMode(buffer.rtexture),
			ClearBackground(bgcolor);
	}
	static void flip() {
		EndTextureMode();
		if (flag_fps) {
			string s = to_string(GetFPS());
			print(s, GetScreenWidth()-font.width(s)-2, 2, GREEN);
		}
		if (buffer.valid())
			buffer.paint(0, 0);
		EndDrawing();  // flip
		begin();  // begin drawing mode for next frame
	}

	// -- Assets --
	static int loadtexture(const string& alias, const string& fname) { return assets.loadtexture(alias, fname); }
	static void unload(const string& alias) { return assets.unload(alias); }
	static Texture& gettexture(const string& alias) { return assets.gettexture(alias); }

	// -- Basic Drawing --
	static void print(const string& str, int x, int y, Color col=WHITE) {
		font.print(str, x, y, col);
	}
	static void text(const string& str, int x, int y, Color col=WHITE) {
		DrawText(str.c_str(), x, y, 10, col);
	}
	static void blitt(Texture2D texture, int tsize, int tile, int x, int y, Color blend=WHITE) {
		if (!IsTextureValid(texture))  return;
		int tx = tile % (texture.width / tsize);
		int ty = tile / (texture.width / tsize);
		float t = tsize;
		Rectangle src{ tx*t, ty*t, t, t };
		Vector2   dst{ float(x), float(y) };
		DrawTextureRec(texture, src, dst, blend);
	}
	static void blittr(Texture2D texture, int tsize, int tile, int x, int y, float rot, Color blend=WHITE) {
		if (!IsTextureValid(texture))  return;
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
