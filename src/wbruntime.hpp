#pragma once
#include "wbproject.hpp"
#include <map>
#include <format>

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
			printf("L%02d: ", lpos+1), tok.show();
			// println(format( "L{:02}: {}", lpos+1, tok.showstr(1) ));

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
			} else if (accept("dim") || accept("let")) {
				bool isdim = tok.presult.at(0) == "dim";
				require("$identifier =");
				string id = tok.presult.at(0);
				int i = pexpr();
				require("$eof");
				if ( isdim &&  globals.count(id))  syntaxerror();
				if (!isdim && !globals.count(id))  syntaxerror();
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
		int i = 0;
		if      (accept("$number"))      i = stoi(tok.presult.at(0));
		else if (accept("$identifier"))  i = stackget(tok.presult.at(0));
		else    return false;
		// TODO: order-of-prescedence, brackets, strings
		while (accept("+") || accept("-") || accept("*") || accept("/")) {
			auto op = tok.presult.at(0);
			int n = 0;
			if      (accept("$number"))      n = stoi(tok.presult.at(0));
			else if (accept("$identifier"))  n = stackget(tok.presult.at(0));
			else    syntaxerror();
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
		println();
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
		print(str);
		lines.push_back("");
	}

	int stackget(const string& id) {
		if (!globals.count(id))  syntaxerror();
		return globals.at(id);
	}
};
