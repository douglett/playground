#pragma once
#include "paintable.hpp"

struct GFXBuffer : Paintable {
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
		if (scaletype == SCALE_DEFAULT) {
			DrawTextureRec(tex, { 0, 0, (float)tex.width, (float)-tex.height }, { (float)offx+y, (float)offy+y }, WHITE);
		} else if (scaletype == SCALE_PX) {
			int screenw = GetScreenWidth();
			int screenh = GetScreenHeight();
			int scale = min( max(screenw/tex.width, 1), max(screenh/tex.height, 1) );
			int xx = (screenw - (tex.width *scale)) / 2;
			int yy = (screenh - (tex.height*scale)) / 2;
			DrawTexturePro(tex,
                Rectangle{ 0, 0, (float)tex.width, (float)-tex.height },
                Rectangle{ (float)xx, (float)yy, (float)tex.width*scale, (float)tex.height*scale },
                Vector2  {0}, 0, WHITE);
		}
	}
};
