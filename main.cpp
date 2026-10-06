#include <iostream>
#include "src/globals.hpp"
using namespace std;

// define globals
GFX gfx;
WBProject project;
WBParser wbparser;
WBRuntime runtime;

void paintedit() {
	auto& f = gfx.font.font();
	int screenw = gfx.screenw() / f.charw,
		screenh = gfx.screenh() / f.charh,
		lpanelw = 20, 
		rpanelw = screenw - lpanelw - 1,
		marginw = 3;
	
	DrawRectangle(0, 0, gfx.screenw(), f.charh, DARKGRAY);
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
			s = (i+1 < 10 ? " " : "") + to_string(i+1);
			gfx.print(s, (lpanelw+1)*f.charw, (i+1)*f.charh, BLUE);
			gfx.print(lines[i].substr(0, rpanelw-marginw), (lpanelw+1+marginw)*f.charw, (i+1)*f.charh);
		}
	}
}

void paintrun() {
	auto& f = gfx.font.font();
	for (size_t i = 0; i < runtime.history.size(); i++) {
		gfx.print(runtime.history[i], 0, i*f.charh, WHITE);
	}
}

struct VisualIO : WBRuntimeIO {
	virtual int input(string& input) {
		runtime.log("");
		string startln = runtime.history.back();
		input = "";

		while (GetCharPressed() > 0) ; // clear input history

		while (!gfx.shouldquit()) {
			// get keyboard input
			for (int key = GetCharPressed(); key > 0; key = GetCharPressed())
				if (key >= 32 && key <= 125)
					input += (char)key;
			// control characters
			if (IsKeyPressed(KEY_BACKSPACE) && input.size() > 0)
				input.pop_back();
			if (IsKeyPressed(KEY_ENTER)) {
				runtime.history.back() = startln + input;
				runtime.lognl();
				return 1;
			}
			// show output while typing
			runtime.history.back() = startln + input + "_";
			paintrun();
			gfx.flip();
		}
		return 0;
	}
};
VisualIO visualio;

void mainloop() {
	int running = 0;
	gfx.init();
	gfx.resizable();
	GFXBuffer buffer;
	buffer.init(160, 160);

	while (!gfx.shouldquit()) {
		if ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && IsKeyPressed(KEY_ENTER))
			gfx.fullscreen();
		
		if (!running) {
			if (IsKeyPressed(KEY_F5))
				running = true,
				runtime.start();
			// paintedit();
		} else {
			if (IsKeyPressed(KEY_F6))
				running = false;
			// paintrun();
			// runtime.pcontinue();
		}

		running ? paintrun() : paintedit();
		// DrawTexture(gfx.font.getselectedfont().texture, 50, 50, WHITE);
		gfx.flip();
	}

	gfx.destroy();
}

void buffertest() {
	gfx.init();
	GFXBuffer buffer;
	buffer.init(160, 160);

	while (!gfx.shouldquit()) {
		BeginTextureMode(buffer.rtexture);
			ClearBackground(SKYBLUE);
			DrawRectangle(0, 0, 20, 20, RED);
			DrawCircleV({10, 10}, 5, MAROON);
		EndTextureMode();
		
		buffer.paint(50, 50);
		gfx.flip();
	}

	buffer.destroy();
	gfx.destroy();
	cout << buffer.width() << endl;
}

int main() {
	printf("starting Playground...\n");
	
	project.load();
	// iproject.report();
	// iproject.run();
	
	wbparser.parseall();
	runtime.iooverride = &visualio;
	// if (ok)  runtime.start();
	
	// mainloop();
	buffertest();
}
