#pragma once
#include "raylib.h"
#include <memory>
#include <string>
using namespace std;

// paintable object base class
struct Paintable {
	using ptr = shared_ptr<Paintable>;
	string id;
	int x = 0, y = 0, z = 0;
	virtual void paint (int x, int y) {}
	virtual void update() {}
};
