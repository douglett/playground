#pragma once
#include "paintable.hpp"

struct Sprite : Paintable {
	int tsize = 16, tile = 0;
	Texture2D texture;
	Color blend = WHITE;
	float rot = 0;

	virtual void paint(int xoff, int yoff) {
		Screen::blittr(texture, tsize, tile, xoff+x, yoff+y, rot, blend);
	}

	int  tx() { return x / tsize; }
	int  ty() { return y / tsize; }
	void tpos(int tx, int ty) { x = tx*tsize, y = ty*tsize; }
};
