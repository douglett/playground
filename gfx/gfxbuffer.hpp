#pragma once
#include "paintable.hpp"

struct GFXBuffer : Paintable {
	enum SCALE_T { SCALE_NONE, SCALE_PX, SCALE_STRETCH, SCALE_STRETCH_FIT };

	SCALE_T scaletype = SCALE_NONE;
	RenderTexture2D rtexture = {0};

	void init(int width, int height, SCALE_T mscale=SCALE_NONE) {
		rtexture = LoadRenderTexture(width, height);
		scaletype = mscale;
	}
	void destroy() {
		UnloadRenderTexture(rtexture);
		rtexture = {0};
	}

	int width()  { return rtexture.texture.width; }
	int height() { return rtexture.texture.height; }

	virtual void paint(int offx, int offy) {
		if (!IsRenderTextureValid(rtexture))  return;
		auto& tex = rtexture.texture;
		DrawTextureRec(tex, { 0, 0, (float)tex.width, (float)-tex.height }, { (float)offx+y, (float)offy+y }, WHITE);
	}
};
