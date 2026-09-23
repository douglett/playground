#pragma once
#include <fstream>

// source file base class
struct SourceFile {
	string fpath, fname;
	bool loaded = false;
	vector<string> lines;

	string fullpath() { return fpath + fname; }

	int load() {
		fstream fs(fullpath(), ios::in);
		if (!fs.is_open())
			return fprintf(stderr, "error opening file: '%s'\n", fullpath().c_str()), 1;
		printf("opened source file: '%s'\n", fname.c_str());
		string line;
		while (getline(fs, line))
			lines.push_back(line);
		return 0;
	}

	vector<string> split(const string& str) {
		string s;
		vector<string> vs;
		stringstream ss(str);
		while (ss >> s)  vs.push_back(s);
		return vs;
	}
};

// basic script file parse & interpret
struct BasScript : SourceFile {
	int lpos = 0;

	int run() {
		while (lpos >= 0 && lpos < (int)lines.size()) {
			auto tok = split(lines[lpos]);
			cout << "    > ";
			for (auto t : tok)
				printf("'%s' ", t.c_str());
			cout << endl;
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
		int err = SourceFile::load();
		if (err)  return err;

		// load & parse project file 
		for (int i = 0; i < (int)lines.size(); i++) {
			string s = clamp(lines[0]);
			if (s.length() == 0 || s[0] == '#') ;
			else if (lines[i][0] == '\t')  printf("unexpected indentation on line %d\n", i);
			else if (s == "name:") ;  // todo
			else if (s == "version:") ;  // todo
			else if (s == "source:") {
				for (i++; i < (int)lines.size(); i++) {
					if (lines[i].length() == 0 || lines[i][0] != '\t')  break;
					srcfiles.push_back({ fpath, clamp(lines[i]) });
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

	string clamp(const string& s) {
		int i = 0, j = int(s.length())-1;
		for (; i < (int)s.length(); i++)
			if (!isspace(s[i]))  break;
		for (; j >= 0; j--)
			if (!isspace(s[j]))  break;
		return s.substr(i, j-i+1);
	}

	void report() {
		printf("iproject: %s (%s)\n", fname.c_str(), fullpath().c_str());
		printf("source:\n");
		for (const auto& f : srcfiles)
			printf("  %s\n", f.fname.c_str());
	}
};
