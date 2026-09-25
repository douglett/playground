#pragma once
#include "wbproject.hpp"

extern WBProject project;

struct WBRuntime {
	vector<string> lines;
	int fpos = 0, lpos = 0;

	int run() {
		reset();
		if (project.srcfiles.size() == 0)
			return println("[no source files]"), 0;
		// println("[program start]");
		printf("[program start]\n");
		
		auto& lines = project.srcfiles[0].lines;
		Tokenizer tok;
		while (lpos >= 0 && lpos < (int)lines.size()) {
			tok.reset();
			tok.tokenizeline(lines[lpos]);
			println(tok.showstr(1));
			lpos++;
		}
		
		println();
		println("[program end]");
		return 0;
	}

	void reset() {
		lines = {};
		fpos = lpos = 0;
	}

	void print(const string& str="") {
		if (lines.size() == 0)  lines.push_back("");
		lines.back() += str;
	}
	void println(const string& str="") {
		lines.push_back(str);
	}
};
