#pragma once
#include "wbproject.hpp"
#include <map>
#include <variant>
// #include <format>

extern WBProject project;

struct WBParserBase {
	struct wexprop;
	struct wwhile;
	struct wvar     { string id; };
	using  watom    = variant<int, string, wvar>;
	using  wexpr    = variant<watom, wexprop>;
	struct wexprop  { string op; vector<wexpr> ab; };
	struct wprint   { int lpos; vector<watom> list; };
	struct wdim     { int lpos; string id; wexpr ex; };
	struct wlet     { int lpos; string id; wexpr ex; };
	struct winput   { int lpos; string id; };
	using  wstmt    = variant<wprint, wdim, wlet, wwhile, winput>;
	using  wblock   = vector<wstmt>;
	struct wwhile   { int lpos; wexpr ex; wblock block; };
	struct wfunc    { int lpos; string name; wblock block; };
};

struct WBParser : WBParserBase {
	Tokenizer tok;
	map<string, wfunc> functions;
	string errormsg;

	int parseall() {
		tok.reset(), functions = {}, errormsg = "";
		if (project.srcfiles.size() == 0)
			return logerr("[-no source files-]");
		return pfile(project.srcfiles.at(0));
	}

	int logerr (const string& err)  { return errormsg = err, fprintf(stderr, "parser_error: %s\n", err.c_str()), false; }
	int peek   (const string& rule) { return tok.peek(rule); }
	int accept (const string& rule) { return tok.accept(rule); }
	int require(const string& rule) { return tok.require(rule) ? true : syntaxerror(); }

	// -- Parsing Structures --
	int pfile(const SourceFile& src) {
		printf("parsing source file: %s\n", src.fname.c_str());
		for (auto& ln : project.srcfiles.at(0).lines)
			tok.tokenizeline(ln);
		// tok.show();
		// parse file top
		try {
			while (!tok.eof())
				if      (accept("$eol")) ;
				else if (pfunc()) ;
				else    syntaxerror();
		} catch(runtime_error& e) {
			return false;
		}
		// OK
		// printf("parse OK\n");
		return true;
	}

	int pfunc() {
		if (!accept("function"))  return false;
		require("$identifier");
		string id = tok.presult.at(0);
		if (functions.count(id))  syntaxerror();
		auto& func = functions[id] = { tok.linepos(), id };
		// arguments
		require("( ) $eol");
		// function block
		return pblock(func.block);
	}

	int pblock(wblock& block) {
		// parse statements
		while (!tok.eof())
			if      (accept("$eol")) ;
			else if (accept("end $eol"))  return true;
			else if (pprint(block)) ;
			else if (pdim(block)) ;
			else if (plet(block)) ;
			else if (pwhile(block)) ;
			else if (pinput(block)) ;
			else    break;
		return syntaxerror();
	}

	int pprint(wblock& block) {
		if (!accept("print"))  return false;
		block.push_back(wprint{ tok.linepos() });
		auto& pr = get<wprint>(block.back());
		// parse each item to print
		while (!tok.eof()) {
			// printf("%s\n", tok.peek().c_str());
			if      (peek("$eol"))  break;
			else if (accept("$strlit"))      pr.list.push_back( tok.stripliteral(tok.presult.at(0)) );
			else if (accept("$number"))      pr.list.push_back( stoi(tok.presult.at(0)) );
			else if (accept("$identifier"))  pr.list.push_back( wvar{ tok.presult.at(0) } );
			else    syntaxerror();
			if (!accept(","))  break;
		}
		require("$eol");
		return true;
	}

	int pdim(wblock& block) {
		if (!accept("dim"))  return false;
		block.push_back(wdim{ tok.linepos() });
		auto& dim = get<wdim>(block.back());
		// dim expression
		require("$identifier =");
		dim.id  = tok.presult.at(0);
		pexpr(dim.ex) || syntaxerror();
		require("$eol");
		return true;
	}

	int plet(wblock& block) {
		if (!accept("let"))  return false;
		block.push_back(wlet{ tok.linepos() });
		auto& let = get<wlet>(block.back());
		// let expression
		require("$identifier =");
		let.id  = tok.presult.at(0);
		pexpr(let.ex) || syntaxerror();
		require("$eol");
		return true;
	}

	int pwhile(wblock& block) {
		if (!accept("while"))  return false;
		block.push_back(wwhile{ tok.linepos() });
		auto& wwl = get<wwhile>(block.back());
		// while expression
		pexpri(wwl.ex) || syntaxerror();
		require("$eol");
		// while block
		pblock(wwl.block);
		return true;
	}

	int pinput(wblock& block) {
		if (!accept("input"))  return false;
		block.push_back(winput{ tok.linepos() });
		auto& inp = get<winput>(block.back());
		// input var
		require("$identifier $eol");
		inp.id = tok.presult.at(0);
		return true;
	}

	// -- Expressions --
	int pexpr(wexpr& ex) {
		// string expressions
		if (accept("$strlit"))
			return ex = tok.stripliteral(tok.presult.at(0)), true;
		// int expressions
		return pexpri(ex);
	}

	// int expressions
	int pexpri(wexpr& ex) {
		return pxcompare(ex);
	}
	int pxcompare(wexpr& ex) {
		if (!pxadd(ex))  return false;
		if (accept("< =") || accept("<")) {
			auto& ex2 = pxconvertprop(ex);
			pexpri(ex2.ab.at(1)) || syntaxerror();
		}
		return true;
	}
	int pxadd(wexpr& ex) {
		if (!pxmul(ex))  return false;
		if (accept("+") || accept("-")) {
			auto& ex2 = pxconvertprop(ex);
			pexpri(ex2.ab.at(1)) || syntaxerror();
		}
		return true;
	}
	int pxmul(wexpr& ex) {
		// TODO: messy
		watom a;
		if (!pxatom(a))  return false;
		ex = a;
		if (accept("*") || accept("/")) {
			auto& ex2 = pxconvertprop(ex);
			pexpri(ex2.ab.at(1)) || syntaxerror();
		}
		return true;
	}
	int pxatom(watom& a) {
		if      (accept("$number"))      return a = stoi(tok.presult.at(0)), true;
		else if (accept("$strlit"))      return a = tok.stripliteral(tok.presult.at(0)), true;
		else if (accept("$identifier"))  return a = wvar{ tok.presult.at(0) }, true;
		return false;
	}
	wexprop& pxconvertprop(wexpr& ex) {
		ex = wexprop{ tok.joinstr(tok.presult, ""), { ex, 0 } };
		return get<wexprop>(ex);
	}

	// -- Errors --
	int syntaxerror() {
		logerr("syntax error, line " + to_string(tok.linepos()));
		throw runtime_error("syntaxerror");
	}
};