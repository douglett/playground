#pragma once
#include "wbproject.hpp"
// #include <map>
// #include <format>
// #include <variant>

extern WBProject project;

struct WBParser {
	// vector<string> logout;
	Tokenizer tok;

	int parseall() {
		if (project.srcfiles.size() == 0)
			return logerr("[-no source files-]");
		return pfile(project.srcfiles.at(0));
	}

	int log    (const string& err)  { return printf("[-%s-]\n", err.c_str()), true; }
	int logerr (const string& err)  { return fprintf(stderr, "[-%s-]\n", err.c_str()), false; }
	int peek   (const string& rule) { return tok.peek(rule); }
	int accept (const string& rule) { return tok.accept(rule); }
	int require(const string& rule) { return tok.accept(rule) ? true : syntaxerror(); }

	// -- Parsing Structures --
	int pfile(const SourceFile& src) {
		log("parsing source file: " + src.fname);
		for (auto& ln : project.srcfiles.at(0).lines)
			tok.tokenizeline(ln);
		tok.show();
		// parse file top
		try {
			while (!tok.eof())
				if      (accept("$eol")) ;
				else if (peek("function"))  pfunc();
				else    syntaxerror();
		} catch(runtime_error& e) {
			return false;
		}
		// OK
		log("parse OK");
		return true;
	}

	int pfunc() {
		require("function $identifier ( ) $eol");
		string id = tok.presult.at(1);
		while (!tok.eof())
			if      (accept("$eol")) ;
			else if (accept("end $eol"))  return true;
			else     syntaxerror();
		return syntaxerror();
	}

	// -- Errors --
	int syntaxerror() {
		logerr("syntax error, line " + to_string(tok.linepos()));
		throw runtime_error("syntaxerror");
	}
};