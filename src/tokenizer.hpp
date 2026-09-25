#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

struct Tokenizer {
	struct Tok { string str; int lpos; int hpos; };
	const Tok TOK_EOF = { "$EOF", -1, -1 };
	const int flag_eol = 0;
	const string lcomment = "#";
	vector<Tok> tok;
	vector<string> presult;
	string fname, errormsg;
	int loglevel = 4, pos = 0, plinepos = 0, presultline = 0;

	// === Tokenize File ===

	int tokenize(const string& fname) {
		log(2, "loading file (" + fname + ")...");
		fstream fs(fname, ios::in);
		if (!fs.is_open())
			return error("opening file: " + fname);
		// setup
		reset();
		this->fname = fname;
		string line;
		log(2, "tokenizing file...");
		// parse line-by-line
		while (getline(fs, line))
			if (!tokenizeline(line))
				return false;
		// ok
		log(2, "tokenizing OK!");
		return true;
	}

	int tokenizeline(const string& line) {
		plinepos++;
		int hpos = 0;
		string t;
		#define addtok() ( t.length() ? tok.push_back({ t, plinepos, hpos }), t = "", 1 : 0 )
		// parse line
		for (size_t i = 0; i < line.length(); i++) {
			char c = line[i];
			hpos++;
			// spaces
			if (isspace(c))  addtok();
			// line comments (exit and ignore)
			else if (lcomment.length() && lcomment[0] == c && line.substr(i, lcomment.length()) == lcomment)  break;
			// string
			else if (c == '"') {
				addtok(), t = c;
				for (i++; i < line.length() && line[i] != '"'; i++)
					t += line[i];
				t += '"';
				if (i >= line.length())
					return error("unterminated string, line " + to_string(plinepos));
			}
			// special characters
			else if (!isalphanum(c))  addtok(), t += c, addtok();
			// normal characters
			else  t += c;  
		}
		// final token (if necessary)
		addtok();
		// add EOL token, if required
		if (flag_eol)  t = "$EOL", addtok();
		return true;
	}

	// helpers
	void reset() {
		tok = {}, fname = "", errormsg = "", pos = plinepos = 0;
	}
	int log(int level, const string& msg) {
		if (loglevel >= level)
			printf("[Tokenizer] %s\n", msg.c_str());
		return true;
	}
	int error(const string& msg) {
		// errormsg = msg + " (line-" + to_string(tok.linepos()) + " @ '" + tok.peek() + "')";
		errormsg = msg;
		if (loglevel >= 1)
			fprintf(stderr, "[Tokenizer] error: %s\n", errormsg.c_str());
		return false;
	}
	void show(int fmt=0) {
		string fs = fmt ? "'" : "";
		cout << "tokens: ";
		for (const auto& t : tok)
			cout << fs << t.str << fs << " ";
		cout << endl;
	}
	string showstr() {
		string s = "tokens: ";
		for (const auto& t : tok)
			s += t.str + " ";
		return s;
	}

	// parsing primatives
	int eof() {
		return pos < 0 || pos >= (int)tok.size();
	}
	int linepos() {
		return eof() ? TOK_EOF.lpos : tok[pos].lpos;
	}
	const string& peek() {
		return eof() ? TOK_EOF.str : tok[pos].str;
	}
	const string& get() {
		return eof() ? TOK_EOF.str : tok[pos++].str;
	}

	// complex parsing
	int accept(const string& rulestr) {
		auto rulelist = splitstr(rulestr);
		if (rulelist.size() < 1)
			return error("empty rule");
		presult = {};
		presultline = linepos();
		int mpos = pos;
		for (const auto& rule : rulelist)
			if (rule == "$eof" && eof())
				presult.push_back(get());
			else if (rule == "$identifier" && isidentifier(peek()))
				presult.push_back(get());
			else if (rule == "$number" && isnumber(peek()))
				presult.push_back(get());
			else if (rule == "$strlit" && isliteral(peek()))
				presult.push_back(get());
			else if (peek() == rule)
				presult.push_back(get());
			else
				return pos = mpos, false;
		return true;
	}

	int require(const string& rule) {
		if (accept(rule))
			return true;
		return error("expected '" + rule + "'");
	}

	int peek(const string& rule) {
		int mpos = pos;
		int ok = accept(rule);
		return pos = mpos, ok;
	}

	// === Token Helpers ===

	// Characters
	static int isalphanum(char c) {
		return isalnum(c) || c == '_';
	}
	static int isnumber(const string& s) {
		for (auto c : s)
			if (!isdigit(c))  return 0;
		return 1;
	}
	static int isidentifier(const string& s) {
		if (s.length() == 0)  return 0;
		if (!isalpha(s[0]) && s[0] != '_')  return 0;
		for (size_t i = 1; i < s.length(); i++)
			if ( !isalphanum(s[i]) )  return 0;
		return 1;
	}
	static int isliteral(const string& s) {
		return s.length() >= 2 && s.front() == '"' && s.back() == '"';
	}
	// static int isarray(const string& s) {
	// 	return s.substr(s.length()-2, 2) == "[]" && isidentifier(s.substr(0, s.length()-2));
	// }
	// static string basetype(const string& type) {
	// 	return isarray(type) ? type.substr(0, type.length()-2) : type;
	// }
	// Strings
	static string stripliteral(const string& str) {
		return isliteral(str) ? str.substr(1, str.length()-2) : str;
	}
	static string escapeliteral(const string& str) {
		string s;
		for (auto c : str)
			if      (c == '\n')  s += "\\n";
			else if (c == '"')  s += "\\\"";
			else    s += c;
		return s;
	}
	static vector<string> splitstr(const string str) {
		vector<string> vs;
		string s;
		for (auto c : str)
			if      (isspace(c) && s.length())  vs.push_back(s), s = "";
			else if (isspace(c)) ;
			else    s += c;
		if (s.length())  vs.push_back(s);
		return vs;
	}
	static string joinstr(const vector<string>& vs, const string& glue = " ") {
		string str;
		for (auto& s : vs)
			str += (str.length() ? glue : "") + s;
		return str;
	}
	static string clamp(const string& s) {
		int i = 0, j = int(s.length())-1;
		for (; i < (int)s.length(); i++)
			if (!isspace(s[i]))  break;
		for (; j >= 0; j--)
			if (!isspace(s[j]))  break;
		return s.substr(i, j-i+1);
	}
};
