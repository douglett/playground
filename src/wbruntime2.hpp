#pragma once

extern WBParser wbparser;

struct WBRuntime2 : WBParserBase {
	vector<string> history;

	int start() {
		if (wbparser.errormsg.length())
			return logerr(wbparser.errormsg);
		if (!wbparser.functions.count("main"))
			return logerr("no function found: main");
		printf("running function 'main'...\n");
		return rfunc(wbparser.functions.at("main"));
	}

	// -- Logging --
	void log(int i) { log(to_string(i)); }
	void log(const string& msg) {
		if (!history.size())  history.push_back("");
		history.back() += msg + " ";
		printf("%s ", msg.c_str());
	}
	void lognl() {
		history.push_back("");
		printf("\n");
	}
	int logerr(const string& err) {
		history.push_back("[-" + err + "-]");
		fprintf(stderr, "%s\n", err.c_str());
		return false;
	}

	// -- Errors --
	int runtimeerror(int lpos) {
		logerr("runtime error, line " + to_string(lpos));
		throw runtime_error("syntaxerror");
	}

	// -- Run --
	int rfunc(const wfunc& fn) {
		for (auto& stmt : fn.block)
			if (const wprint* pr = get_if<wprint>(&stmt))  rprint(*pr);
		return true;
	}

	int rprint(const wprint& pr) {
		for (auto& v : pr.list)
			if      (auto* s = get_if<string>(&v))  log(*s);
			else if (auto* i = get_if<int>(&v))     log(*i);
			else    runtimeerror(pr.lpos);
		lognl();
		return true;
	}
};
