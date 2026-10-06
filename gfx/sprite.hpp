#pragma once
#include "paintable.hpp"
#include "assets.hpp"

struct Sprite : Paintable {
	int originx=0, originy=0, width=16, height=16;
	string textureid;
	Color blend = WHITE;
	float rot = 0;

	void source(int ox, int oy, int w, int h) { source(textureid, ox, oy, w, h); }
	void source(const string& texid, int ox, int oy, int w, int h) {
		textureid = texid, originx = ox, originy = oy, width = w, height = h;
	}
	void tsource(int tile) { tsource(textureid, width, tile); }
	void tsource(const string& texid, int tsize, int tile) {
		auto& texture = Assets::gettexture(texid);
		int tx = (tile % (texture.width / tsize));
		int ty = tile / (texture.width / tsize);
		textureid = texid, originx = tx*tsize, originy = ty*tsize, width = tsize, height = tsize;
	}

	virtual void paint(int xoff, int yoff) {
		Rectangle src{ float(originx), float(originy), float(width), float(height) };
		Vector2   ori{ float(width) / 2, float(height) / 2 };
		Rectangle dst{ xoff+x+ori.x, yoff+y+ori.y, float(width), float(height) };
		DrawTexturePro(Assets::gettexture(textureid), src, dst, ori, rot, blend);
	}
};
