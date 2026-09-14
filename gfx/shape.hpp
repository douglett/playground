#pragma once
#include "paintable.hpp"

struct Shape : Paintable {
	Color color = WHITE;
};

struct ShapeRectangle : Shape {
	int width = 10, height = 10;

	virtual void paint(int xoff, int yoff) {
		DrawRectangle(xoff+x, yoff+y, width, height, color);
	}
};

struct ShapeCircle : Shape {
	float radius = 0;

	virtual void paint(int xoff, int yoff) {
		DrawCircle(xoff+x, yoff+y, radius, color);
	}
};
