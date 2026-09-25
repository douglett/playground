#pragma once
#include <fstream>
#include "tokenizer.hpp"

// source file base class
struct SourceFile {
	string fpath, fname;
	bool loaded = false;
	vector<string> lines;

	string fullpath() { return fpath + fname; }

	int load() {
		lines = {}, loaded = false;
		fstream fs(fullpath(), ios::in);
		if (!fs.is_open())
			return fprintf(stderr, "error opening file: '%s'\n", fullpath().c_str()), 1;
		printf("opened source file: '%s'\n", fname.c_str());
		string line;
		while (getline(fs, line))
			lines.push_back(line);
		loaded = true;
		return 0;
	}
};

// basic script file parse & interpret
struct BasScript : SourceFile {
	int lpos = 0;

	int run() {
		Tokenizer tok;
		while (lpos >= 0 && lpos < (int)lines.size()) {
			tok.reset();
			tok.tokenizeline(lines[lpos]);
			tok.show(1);
			lpos++;
		}
		return 0;
	}
};

// interpreter project
struct IProject : SourceFile {
	vector<BasScript> srcfiles;

	int load() {
		fpath = "./wbprojects/";
		fname = "test.wbproj";
		srcfiles = {};  // reset
		int err = SourceFile::load();
		if (err)  return err;

		// load & parse project file 
		for (int i = 0; i < (int)lines.size(); i++) {
			string s = Tokenizer::clamp(lines[0]);
			if (s.length() == 0 || s[0] == '#') ;
			else if (lines[i][0] == '\t')  printf("unexpected indentation on line %d\n", i);
			else if (s == "name:") ;  // todo
			else if (s == "version:") ;  // todo
			else if (s == "source:") {
				for (i++; i < (int)lines.size(); i++) {
					if (lines[i].length() == 0 || lines[i][0] != '\t')  break;
					srcfiles.push_back({ fpath, Tokenizer::clamp(lines[i]) });
				}
			}
			else
				return fprintf(stderr, "unknown command on line %d: '%s'\n", i, s.c_str()), 2;
		}

		// load wbas source files
		for (auto& s : srcfiles) {
			int err = s.load();
			if (err)  return err;
		}

		return 0;
	}

	int run() {
		for (auto& src : srcfiles) {
			int err = src.run();
			if (err)  return err;
		}
		return 0;
	}

	void report() {
		printf("iproject: %s (%s)\n", fname.c_str(), fullpath().c_str());
		printf("source:\n");
		for (const auto& f : srcfiles)
			printf("  %s\n", f.fname.c_str());
	}

	vector<string> reports() {
		vector<string> vs;
		vs.push_back("iproject: " + fname + " (" + fullpath() +")");
		vs.push_back("source:");
		for (const auto& f : srcfiles)
			vs.push_back("  " + f.fname);
		return vs;
	}
};
