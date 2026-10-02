#pragma once

extern WBParser wbparser;

struct WBRuntime2 : WBParserBase {
	vector<string> history;
	map<string, int> memory;

	int start() {
		memory = {};
		if (wbparser.errormsg.length())
			return logerr(wbparser.errormsg);
		if (!wbparser.functions.count("main"))
			return logerr("no function found: main");
		printf("running function 'main'...\n");
		rfunc(wbparser.functions.at("main"));
		return true;
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
	int memoryerror(int lpos) {
		logerr("memory error, line " + to_string(lpos));
		throw runtime_error("memoryerror");
	}

	// -- Run --
	void rfunc(const wfunc& fn) {
		for (auto& stmt : fn.block)
			if      (auto* st = get_if<wprint>(&stmt))  rprint(*st);
			else if (auto* st = get_if<wdim>(&stmt))    rdim(*st);
			else if (auto* st = get_if<wlet>(&stmt))    rlet(*st);
			else    runtimeerror(fn.lpos); // warning: this will be wrong, but shouldn't be run
	}

	void rprint(const wprint& pr) {
		for (auto& arg : pr.list)
			if      (auto* s = get_if<string>(&arg))  log(*s);
			else if (auto* i = get_if<int>(&arg))     log(*i);
			else if (auto* v = get_if<wvar>(&arg))    log(getmemi(v->id, pr.lpos));
			else    runtimeerror(pr.lpos);
		lognl();
	}

	void rdim(const wdim& dim) {
		if (memory.count(dim.id))  memoryerror(dim.lpos);
		memory[dim.id] = dim.val;
	}

	void rlet(const wlet& let) {
		if (!memory.count(let.id))  memoryerror(let.lpos);
		memory.at(let.id) = let.val;
	}

	// -- Memory --
	int& getmemi(const string& id, int lpos=-1) {
		if (!memory.count(id))  memoryerror(lpos);
		return memory[id];
	}
};
