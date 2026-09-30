#pragma once
#include "wbproject.hpp"
#include <map>
#include <format>
#include <variant>

extern WBProject project;

struct WBRuntime {
	enum STATE_T { STATE_IDLE, STATE_RUNNING, STATE_END, STATE_ERROR, STATE_INPUT };
	typedef variant<int, string> Mem_T;
	struct ControlPoint { string type; int lpos; };

	vector<string> lines;
	map<string, Mem_T> globals;
	vector<ControlPoint> ctrl;
	Tokenizer tok;
	int fpos = 0, lpos = 0;
	STATE_T state = STATE_IDLE;
	struct { string id, startln, input; } input;

	int start() {
		// reset
		lines = {}, globals = {}, tok.reset();
		fpos = lpos = 0;
		state = STATE_IDLE;
		// indicate start
		if (project.srcfiles.size() == 0)
			return println("[no source files]"), 0;
		printf("[program start]\n");
		// run block
		// try { return pcontinue(); }
		// catch (runtime_error& e) { return 1; }
		state = STATE_RUNNING;
		return pcontinue();
	}

	int pcontinue() {
		if (state == STATE_INPUT)    return rinput(), true;
		if (state != STATE_RUNNING)  return false;
		try {
			return pline();
		} catch (runtime_error& e) {
			state = STATE_ERROR;
			return false;
		}
	}

	// -- Parse Helpers --
	int peek   (const string& rule) { return tok.peek(rule); }
	int accept (const string& rule) { return tok.accept(rule); }
	int require(const string& rule) { if (!tok.require(rule)) syntaxerror(); return true; }

	// -- Parse Statements --
	int pline() {
		auto& lines = project.srcfiles.at(fpos).lines;
		if (lpos >= (int)lines.size()) {
			println(), println("[-program end-]");
			return state = STATE_END, false;
		}
		// parse line
		tok.reset(), tok.tokenizeline(lines[lpos]);
		printf("L%02d: ", lpos+1), tok.show();
		// println(format( "L{:02}: {}", lpos+1, tok.showstr(1) ));
		// run statement
		if      (accept("$eof")) ;
		else if (pprint()) ;
		else if (pdim()) ;
		else if (plet()) ;
		else if (pinput()) ;
		else if (pwhile()) ;
		else if (pend()) return true;
		else    syntaxerror();
		// next
		lpos++;		
		return true;
	}

	int pdim() {
		if (!accept("dim"))  return false;
		require("$identifier =");
		string id = tok.presult.at(0);
		if      (globals.count(id))     memoryerror();
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
		input.id = tok.presult.at(0);
		memgets(input.id);  // validate
		state = STATE_INPUT;
		print(), input.input = "", input.startln = lines.back();
		return rinput(), true;
	}

	int pwhile() {
		if (!accept("while"))  return false;
		int i = 0;
		if (!pexpr(i))  syntaxerror();
		require("$eof");
		ctrlstart("while");
		if (!i)
			ctrljumpend("while");
		return true;
	}

	int pend() {
		if (!accept("end"))  return false;
		require("$eof");
		ctrlend();
		return true;
	}

	int pexpr(int& i) {
		if      (accept("$number"))      i = stoi(tok.presult.at(0));
		else if (accept("$identifier"))  i = memgeti(tok.presult.at(0));
		else    return false;
		// TODO: order-of-prescedence, brackets
		while (accept("+") || accept("-") || accept("*") || accept("/") || accept("< =")) {
			auto op = tok.joinstr(tok.presult, "");
			int n = 0;
			if      (accept("$number"))      n = stoi(tok.presult.at(0));
			else if (accept("$identifier"))  n = memgeti(tok.presult.at(0));
			else    syntaxerror();
			if      (op == "+" )  i += n;
			else if (op == "-" )  i -= n;
			else if (op == "*" )  i *= n;
			else if (op == "/" )  i /= n;
			else if (op == "<=")  i = i <= n;
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
		println(), println("[-syntax error on line " + to_string(lpos+1) + "-]");
		throw runtime_error(lines.back());
	}
	void memoryerror() {
		println(), println("[-incorrect memory access on line " + to_string(lpos+1) + "-]");
		throw runtime_error(lines.back());
	}
	void controlerror() {
		println(), println("[-control structure error on line " + to_string(lpos+1) + "-]");
		throw runtime_error(lines.back());
	}

	// -- Runtime IO --
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
	void rinput() {
		// get keyboard input
		for (int key = GetCharPressed(); key > 0; key = GetCharPressed())
			if (key >= 32 && key <= 125)
				input.input += (char)key;
		// control characters
		if (IsKeyPressed(KEY_BACKSPACE))
			input.input.pop_back();
		if (IsKeyPressed(KEY_ENTER)) {
			memgets(input.id) = input.input;
			lines.back() = input.startln + input.input;
			println();
			return state = STATE_RUNNING, void();
		}
		// show output while typing
		lines.back() = input.startln + input.input + "_";
	}

	// -- Runtime Control Structures --
	void ctrlstart(const string& type) {
		if (ctrl.size() && ctrl.back().type == type && ctrl.back().lpos == lpos)  return;
		ctrl.push_back({ type, lpos });
	}
	void ctrlend() {
		if (!ctrl.size())
			controlerror();
		else if (ctrl.back().type == "while")
			lpos = ctrl.back().lpos;
		else
			controlerror();
	}
	void ctrljumpend(const string& type) {
		if (!ctrl.size() || ctrl.back().type != type)  controlerror();
		auto& lines = project.srcfiles.at(fpos).lines;
		while (lpos < (int)lines.size()) {
			tok.reset(), tok.tokenizeline(lines[lpos]), lpos++;
			if (accept("end $eof"))
				return ctrl.pop_back(), void();
		}
		controlerror();
	}
	
	// -- Runtime Memory --
	Mem_T& memget(const string& id) {
		if (!globals.count(id))  memoryerror();
		return globals.at(id);
	}
	int& memgeti(const string& id) {
		static int temp = 0;
		auto& m = memget(id);
		if (int* i = get_if<int>(&m))  return *i;
		return memoryerror(), temp;
	}
	string& memgets(const string& id) {
		static string temp;
		auto& m = memget(id);
		if (string* s = get_if<string>(&m))  return *s;
		return memoryerror(), temp;
	}
};
