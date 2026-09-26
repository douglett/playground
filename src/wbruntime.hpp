#pragma once
#include "wbproject.hpp"
#include <map>

extern WBProject project;

struct WBRuntime {
	vector<string> lines;
	map<string, int> globals;
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
			tok.reset(), tok.tokenizeline(lines[lpos]);
			tok.show();
			// println(tok.showstr(1));

			if (tok.accept("$eof")) ;
			else if (tok.accept("print")) {
				while (!tok.eof())
					if (tok.accept("$number"))
						print(tok.presult.at(0) + " ");
					else if (tok.accept("$strlit"))
						print(tok.stripliteral(tok.presult.at(0)) + " ");
					else if (tok.accept("$identifier"))
						print(stackget(tok.presult.at(0)));
					else
						{ syntaxerror(); break; }
				println();
			} else if (tok.accept("dim")) {
				tok.require("$identifier = $number $eof");
				string id = tok.presult.at(0);
				int    i  = stoi(tok.presult.at(2));
				if (globals.count(id))  syntaxerror();
				globals[id] = i;
			} else {
				syntaxerror();
				break;
			}

			lpos++;
		}
		
		println();
		println("[-program end-]");
		return 0;
	}

	void reset() {
		lines = {};
		fpos = lpos = 0;
	}
	void syntaxerror() {
		println("[-syntax error on line " + to_string(lpos+1) + "-]");
	}

	void print(int i) { print(to_string(i)); }
	void print(const string& str="") {
		if (lines.size() == 0)  lines.push_back("");
		lines.back() += str;
	}
	void println(int i) { println(to_string(i)); }
	void println(const string& str="") {
		lines.push_back(str);
	}

	int stackget(const string& id) {
		if (!globals.count(id))  syntaxerror();
		return globals.at(id);
	}
};
