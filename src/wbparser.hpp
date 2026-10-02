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

	int log   (const string& err) { return printf("[-%s-]\n", err.c_str()), true; }
	int logerr(const string& err) { return fprintf(stderr, "[-%s-]\n", err.c_str()), false; }

	// -- Parsing Structures --
	int pfile(const SourceFile& src) {
		log("parsing source file: " + src.fname);
		for (auto& ln : project.srcfiles.at(0).lines)
			tok.tokenizeline(ln);
		tok.show();
		
		// while (!tok.eof())
		// 	if (tok.peek("$eol"))
		
		log("parse OK");
		return true;
	}
};