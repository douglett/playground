#pragma once
#include "wbproject.hpp"
#include <map>

extern WBProject project;

struct WBRuntime {
	vector<string> lines;
	map<string, int> globals;
	Tokenizer tok;
	int fpos = 0, lpos = 0;

	void reset() {
		lines = {};
		globals = {};
		tok.reset();
		fpos = lpos = 0;
	}

	int run() {
		try { return prun(); }
		catch (runtime_error& e) { return 1; }
	}
	int prun() {
		reset();
		if (project.srcfiles.size() == 0)
			return println("[no source files]"), 0;
		// println("[program start]");
		printf("[program start]\n");
		
		auto& lines = project.srcfiles[0].lines;
		while (lpos >= 0 && lpos < (int)lines.size()) {
			tok.reset(), tok.tokenizeline(lines[lpos]);
			tok.show();
			// println(tok.showstr(1));

			if (accept("$eof")) ;
			else if (accept("print")) {
				while (!tok.eof())
					if (accept("$number"))
						print(tok.presult.at(0) + " ");
					else if (accept("$strlit"))
						print(tok.stripliteral(tok.presult.at(0)) + " ");
					else if (accept("$identifier"))
						print(stackget(tok.presult.at(0)));
					else
						syntaxerror();
				println();
			} else if (accept("dim")) {
				require("$identifier =");
				string id = tok.presult.at(0);
				int i = pexpr();
				require("$eof");
				if (globals.count(id))  syntaxerror();
				globals[id] = i;
			} else {
				syntaxerror();
			}

			lpos++;
		}
		
		println();
		println("[-program end-]");
		return 0;
	}

	int pexpr() {
		if (!accept("$number"))  return false;
		int i = stoi(tok.presult.at(0));
		// TODO: order-of-prescedence, brackets, strings
		while (accept("+") || accept("-") || accept("*") || accept("/")) {
			auto op = tok.presult.at(0);
			require("$number");
			int n = stoi(tok.presult.at(0));
			if      (op == "+")  i += n;
			else if (op == "-")  i -= n;
			else if (op == "*")  i *= n;
			else if (op == "/")  i /= n;
			else    syntaxerror();
		}
		return i;
	}

	int peek   (const string& rule) { return tok.peek(rule); }
	int accept (const string& rule) { return tok.accept(rule); }
	int require(const string& rule) { if (!tok.require(rule)) syntaxerror(); return true; }
	void syntaxerror() {
		println("[-syntax error on line " + to_string(lpos+1) + "-]");
		throw runtime_error(lines.back());
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
