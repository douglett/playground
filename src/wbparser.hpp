#pragma once
#include "wbproject.hpp"
#include <map>
#include <variant>
// #include <format>

extern WBProject project;

struct WBParserBase {
	struct wvar     { string id; };
	using  watom    = variant<string, int, wvar>;
	struct wprint   { int lpos; vector<watom> list; };
	struct wdim     { int lpos; string id; int val; };
	using  wstmt    = variant<wprint, wdim>;
	using  wblock   = vector<wstmt>;
	struct wfunc    { int lpos; string name; wblock block; };
	
	// int log    (const string& err)  { return printf("%s\n", err.c_str()), true; }
	// int logerr (const string& err)  { return errormsg = err, fprintf(stderr, "%s\n", err.c_str()), false; }
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
	int require(const string& rule) { return tok.accept(rule) ? true : syntaxerror(); }

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
		int lpos = tok.linepos();
		require("$identifier ( ) $eol");
		string id = tok.presult.at(0);
		if (functions.count(id))  syntaxerror();
		auto& func = functions[id] = { lpos, id };
		// parse statements
		while (!tok.eof())
			if      (accept("$eol")) ;
			else if (accept("end $eol"))  return true;
			else if (pprint(func.block)) ;
			else if (pdim(func.block)) ;
			else    syntaxerror();
		return syntaxerror();
	}

	int pprint(wblock& block) {
		if (!accept("print"))  return false;
		int lpos = tok.linepos();
		block.push_back(wprint{ lpos });
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
		int lpos = tok.linepos();
		block.push_back(wdim{ lpos });
		auto& dim = get<wdim>(block.back());
		// dim expression
		require("$identifier = $number $eol");
		dim.id  = tok.presult.at(0);
		dim.val = stoi(tok.presult.at(2));
		return true;
	}

	// -- Errors --
	int syntaxerror() {
		logerr("syntax error, line " + to_string(tok.linepos()));
		throw runtime_error("syntaxerror");
	}
};