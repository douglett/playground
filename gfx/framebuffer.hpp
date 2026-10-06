#pragma once
#include "paintable.hpp"

struct FrameBuffer : Paintable {
	enum SCALE_T { SCALE_DEFAULT, SCALE_PX, SCALE_STRETCH, SCALE_STRETCH_FIT };

	SCALE_T scaletype = SCALE_DEFAULT;
	RenderTexture2D rtexture = {0};

	void init(int width, int height, SCALE_T mscale=SCALE_DEFAULT) {
		rtexture = LoadRenderTexture(width, height);
		scaletype = mscale;
	}
	void destroy() {
		UnloadRenderTexture(rtexture);
		rtexture = {0};
	}

	int valid()  { return IsRenderTextureValid(rtexture); }
	int width()  { return rtexture.texture.width; }
	int height() { return rtexture.texture.height; }

	virtual void paint(int offx, int offy) {
		if (!valid())  return;
		auto& tex = rtexture.texture;
		int screenw = GetScreenWidth(), screenh = GetScreenHeight();
		int xx = offx+x, yy = offy+y;
		float scalex = 1, scaley = 1;
		// find drawing position & scale
		if (scaletype == SCALE_PX) {
			scalex = max(screenw/tex.width, 1);
			scaley = max(screenh/tex.height, 1);
			scalex = scaley = min(scalex, scaley);
			xx = (screenw - (tex.width *scalex)) / 2;
			yy = (screenh - (tex.height*scaley)) / 2;
		} else if (scaletype == SCALE_STRETCH) {
			scalex = screenw/float(tex.width);
			scaley = screenh/float(tex.height);
			xx = (screenw - (tex.width *scalex)) / 2;
			yy = (screenh - (tex.height*scaley)) / 2;
		} else if (scaletype == SCALE_STRETCH_FIT) {
			scalex = screenw/float(tex.width);
			scaley = screenh/float(tex.height);
			scalex = scaley = min(scalex, scaley);
			xx = (screenw - (tex.width *scalex)) / 2;
			yy = (screenh - (tex.height*scaley)) / 2;
		}
		// draw
		DrawTexturePro(tex,
			Rectangle{ 0, 0, (float)tex.width, (float)-tex.height },
			Rectangle{ (float)xx, (float)yy, (float)tex.width*scalex, (float)tex.height*scaley },
			Vector2  {0}, 0, WHITE);
	}
};
