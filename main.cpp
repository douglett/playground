#include <iostream>
#include "src/globals.hpp"
using namespace std;

// define globals
GFX gfx;
IProject iproject;

void painttext() {
	auto& f = gfx.font.font();
	int tw = gfx.screen.width / f.charw;
	int th = gfx.screen.height / f.charh;
	// gfx.print(to_string(tw) + " " + to_string(th), 10, 10);

	for (int y = 0; y < th; y++)
	for (int x = 0; x < tw; x++) {
		auto s = x == 0 || y == 0 || x == tw-1 || y == th-1 ? "@" : " ";
		gfx.print(s, x*f.charw, y*f.charh);
	}
}

void paintedit() {
	auto& f = gfx.font.font();
	int screenw = gfx.screen.width / f.charw,
		screenh = gfx.screen.height / f.charh,
		lpanelw = 20, 
		rpanelw = screenw - lpanelw - 1,
		marginw = 3;
	
	string s;
	auto lines = iproject.reports();
	for (size_t i = 0; i < lines.size(); i++)
		gfx.print(lines[i].substr(0, lpanelw), 0, i*f.charh);
	s = string() + char(163);
	for (int i = 0; i < screenh; i++)
		gfx.print(s, lpanelw*f.charw, i*f.charh);
	if (iproject.srcfiles.size()) {
		auto& lines = iproject.srcfiles[0].lines;
		for (size_t i = 0; i < lines.size(); i++) {
			s = (i < 10 ? " " : "") + to_string(i+1);
			gfx.print(s, (lpanelw+1)*f.charw, i*f.charw, BLUE);
			gfx.print(lines[i].substr(0, rpanelw-marginw), (lpanelw+1+marginw)*f.charw, i*f.charh);
		}
	}
}

void mainloop() {
	// setup window
	gfx.screen.width = 800;
	gfx.screen.height = 600;
	gfx.screen.zoom = 1;
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	gfx.init();
	// SetWindowState(FLAG_WINDOW_RESIZABLE);

	while (!gfx.shouldQuit()) {
		if ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && IsKeyPressed(KEY_ENTER))
			gfx.screen.fullscreen();

		// DrawTexture(gfx.qbfont.getselectedfont().texture, 50, 50, WHITE);

		// painttext();
		paintedit();
		gfx.flip();
	}

	gfx.destroy();
}

int main() {
	printf("starting Playground...\n");
	
	iproject.load();
	// iproject.report();
	// iproject.run();
	
	mainloop();
}
