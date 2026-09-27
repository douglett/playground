#pragma once
#include "wbproject.hpp"
#include <map>
#include <format>
#include <variant>

extern WBProject project;

struct WBRuntime {
	typedef variant<int, string> Mem_T;
	vector<string> lines;
	map<string, Mem_T> globals;
	Tokenizer tok;
	int fpos = 0, lpos = 0;

	int run() {
		// reset
		lines = {}, globals = {}, tok.reset();
		fpos = lpos = 0;
		// indicate start
		if (project.srcfiles.size() == 0)
			return println("[no source files]"), 0;
		printf("[program start]\n");
		// run block
		try { return pblock(); }
		catch (runtime_error& e) { return 1; }
	}

	// -- Parse Helpers --
	int peek   (const string& rule) { return tok.peek(rule); }
	int accept (const string& rule) { return tok.accept(rule); }
	int require(const string& rule) { if (!tok.require(rule)) syntaxerror(); return true; }

	// -- Parse Statements --
	int pblock() {
		auto& lines = project.srcfiles[0].lines;
		while (lpos >= 0 && lpos < (int)lines.size()) {
			tok.reset(), tok.tokenizeline(lines[lpos]);
			printf("L%02d: ", lpos+1), tok.show();
			// println(format( "L{:02}: {}", lpos+1, tok.showstr(1) ));
			// run statement
			if      (accept("$eof")) ;
			else if (pprint()) ;
			else if (pdim()) ;
			else if (plet()) ;
			else if (pinput()) ;
			else    syntaxerror();
			// next
			lpos++;
		}
		
		println();
		println("[-program end-]");
		return 0;
	}

	int pdim() {
		if (!accept("dim"))  return false;
		require("$identifier =");
		string id = tok.presult.at(0);
		if      (globals.count(id))     syntaxerror();
		else if (string s;  pexprs(s))  globals[id] = s;
		else if (int i = 0; pexpr(i))   globals[id] = i;
		else    syntaxerror();
		return true;
	}

	int plet() {
		if (!accept("let"))  return false;
		require("$identifier =");
		string id = tok.presult.at(0);
		if      (string s;  pexprs(s))  memgets(id) = s;
		else if (int i = 0; pexpr(i))   memgeti(id) = i;
		else    syntaxerror();
		return true;
	}

	int pprint() {
		if (!accept("print"))  return false;
		while (!tok.eof())
			if (accept("$number"))
				print(tok.presult.at(0) + " ");
			else if (accept("$strlit"))
				print(tok.stripliteral(tok.presult.at(0)) + " ");
			else if (accept("$identifier"))
				print(memget(tok.presult.at(0)));
			else
				syntaxerror();
		println();
		return true;
	}

	int pinput() {
		if (!accept("input"))  return false;
		require("$identifier $eof");
		auto id = tok.presult.at(0);
		syntaxerror();
		return true;
	}

	int pexpr(int& i) {
		if      (accept("$number"))      i = stoi(tok.presult.at(0));
		else if (accept("$identifier"))  i = memgeti(tok.presult.at(0));
		else    return false;
		// TODO: order-of-prescedence, brackets
		while (accept("+") || accept("-") || accept("*") || accept("/")) {
			auto op = tok.presult.at(0);
			int n = 0;
			if      (accept("$number"))      n = stoi(tok.presult.at(0));
			else if (accept("$identifier"))  n = memgeti(tok.presult.at(0));
			else    syntaxerror();
			if      (op == "+")  i += n;
			else if (op == "-")  i -= n;
			else if (op == "*")  i *= n;
			else if (op == "/")  i /= n;
			else    syntaxerror();
		}
		return true;
	}

	int pexprs(string& result) {
		if (accept("$strlit"))
			return result = tok.stripliteral(tok.presult.at(0)), 1;
		return 0;
	}

	// -- Errors --
	void syntaxerror() {
		println();
		println("[-syntax error on line " + to_string(lpos+1) + "-]");
		throw runtime_error(lines.back());
	}

	// -- Runtime Output --
	// void print(int i) { print(to_string(i)); }
	void print(const Mem_T& m) {
		if (const int* i = get_if<int>(&m))
		print(to_string(*i));
		else if (const string* s = get_if<string>(&m))
		print(*s);
	}
	void print(const string& str="") {
		if (lines.size() == 0)  lines.push_back("");
		lines.back() += str;
	}
	void println(const string& str="") {
		print(str);
		lines.push_back("");
	}
	
	// -- Runtime Memory --
	Mem_T& memget(const string& id) {
		if (!globals.count(id))  syntaxerror();
		return globals.at(id);
	}
	int& memgeti(const string& id) {
		static int temp = 0;
		auto& m = memget(id);
		if (int* i = get_if<int>(&m))  return *i;
		return syntaxerror(), temp;
	}
	string& memgets(const string& id) {
		static string temp;
		auto& m = memget(id);
		if (string* s = get_if<string>(&m))  return *s;
		return syntaxerror(), temp;
	}
};
