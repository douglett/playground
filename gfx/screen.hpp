#pragma once
#include "raylib.h"
#include <iostream>
using namespace std;

struct Screen {
	int width = 160, height = 160;
	string winname = "mygame";
	float zoom = 4;
	Color bgcolor = BLACK;
	Camera2D camera = {0};

	int init() {
		SetTraceLogLevel(LOG_WARNING);
		InitWindow(width*zoom, height*zoom, winname.c_str());
		SetTargetFPS(60);
		camera.zoom = zoom;
		// ok
		printf("Screen initialized: %d %d\n", width, height);
		begin();
		return 0;
	}

	void destroy() {
		CloseWindow();  // Close window and OpenGL context
	}

	// begin drawing mode (automatic)
	void begin() {
		BeginDrawing();
		BeginMode2D(camera);
		ClearBackground(bgcolor);
	}

	// flip backbuffer to screen
	void flip() {
		// show framerate
		auto fps = to_string(GetFPS());
		DrawText(fps.c_str(), 1, 1, 10, GREEN);
		// Qbfont.Text(fps, scr.Width - len(fps)*4 - 2, 2, ray.Green)
		// flip
		EndMode2D();
		EndDrawing();
		// begin drawing mode for next frame
		begin();
	}

	static void text(const string& str, int x, int y, Color col=WHITE) {
		DrawText(str.c_str(), x, y, 10, col);
	}

	// blit texture
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
};
