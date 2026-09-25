#include <iostream>
#include "src/globals.hpp"
using namespace std;

// define globals
GFX gfx;
WBProject project;
WBRuntime runtime;

void paintedit() {
	auto& f = gfx.font.font();
	int screenw = gfx.screen.width / f.charw,
		screenh = gfx.screen.height / f.charh,
		lpanelw = 20, 
		rpanelw = screenw - lpanelw - 1,
		marginw = 3;
	
	DrawRectangle(0, 0, gfx.screen.width, f.charh, DARKGRAY);
	gfx.print("f5:run f6:stop", 0, 0, BLUE);
	string s;
	auto lines = project.showstr();
	for (size_t i = 0; i < lines.size(); i++)
		gfx.print(lines[i].substr(0, lpanelw), 0, (i+1)*f.charh);
	s = string() + char(163);
	for (int i = 0; i < screenh; i++)
		gfx.print(s, lpanelw*f.charw, (i+1)*f.charh);
	if (project.srcfiles.size()) {
		auto& lines = project.srcfiles[0].lines;
		for (size_t i = 0; i < lines.size(); i++) {
			s = (i < 10 ? " " : "") + to_string(i+1);
			gfx.print(s, (lpanelw+1)*f.charw, (i+1)*f.charh, BLUE);
			gfx.print(lines[i].substr(0, rpanelw-marginw), (lpanelw+1+marginw)*f.charw, (i+1)*f.charh);
		}
	}
}

void paintrun() {
	auto& f = gfx.font.font();
	for (size_t i = 0; i < runtime.lines.size(); i++) {
		gfx.print(runtime.lines[i], 0, i*f.charh, WHITE);
	}
}

void mainloop() {
	int running = 0;
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
		else if (!running && IsKeyPressed(KEY_F5))
			running = true,
			runtime.run();
		else if (running && IsKeyPressed(KEY_F6))
			running = false;

		// DrawTexture(gfx.qbfont.getselectedfont().texture, 50, 50, WHITE);

		if (running)
			paintrun();
		else
			paintedit();
		gfx.flip();
	}

	gfx.destroy();
}

int main() {
	printf("starting Playground...\n");
	
	project.load();
	// iproject.report();
	// iproject.run();
	
	mainloop();
}
