#pragma once
#include <fstream>

// interpreter project
struct IProject {
	struct File { string path, name; string fname() { return path+name; } };
	File ipfile = { "./wbprojects/", "test.wbproj" };
	vector<File> srcfiles;

	int load() {
		fstream fs(ipfile.fname(), ios::in);
		if (!fs.is_open())
			return fprintf(stderr, "error opening file: '%s'\n", ipfile.fname().c_str()), 1;
		
		printf("opened project file: '%s'\n", ipfile.name.c_str());
		string line, s;
		vector<string> lines;
		while (getline(fs, line))
			lines.push_back(line);
		fs.close();

		stringstream ss;
		for (size_t i = 0; i < lines.size(); i++) {
			ss.str(lines[i]), ss.clear();
			ss >> s;
			if (s.length() == 0 || s[0] == '#') ;
			else if (s == "source:") {
				for (i++; i < lines.size(); i++) {
					if (lines[i].length() == 0 || lines[i][0] != '\t')  break;
					ss.str(lines[i]), ss.clear();
					ss >> s;
					srcfiles.push_back({ ipfile.path, s });
				}
			}
			else
				return fprintf(stderr, "unknown command on line %d: '%s'\n", (int)i, s.c_str()), 2;
		}

		return 0;
	}

	// string strip(const string& s) {
	// 	string r;
	// 	for (int i = 0; i < s.length(); i++)
	// }

	void report() {
		printf("iproject: %s (%s)\n", ipfile.name.c_str(), ipfile.fname().c_str());
		printf("source:\n");
		for (const auto& f : srcfiles)
			printf("  %s\n", f.name.c_str());
	}
};
