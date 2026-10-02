#pragma once

extern WBParser wbparser;

struct WBRuntime2 : WBParserBase {
	using Mem_T = variant<int, string>;

	vector<string> history;
	map<string, Mem_T> memory;
	int lpos = 0;

	int start() {
		lpos = 0, memory = {};
		if (wbparser.errormsg.length())
			return logerr(wbparser.errormsg);
		if (!wbparser.functions.count("main"))
			return logerr("no function found: main");
		try {
			printf("running function 'main'...\n");
			rfunc(wbparser.functions.at("main"));
			lognl(), log("[-program end-]");
			return true;
		} catch(runtime_error& e) {
			return false;
		}
	}

	// -- Logging --
	void log(int i) { log(to_string(i)); }
	// void log(const Mem_T& v) {
	// 	if      (auto* vv = get_if<int>(&v))     log(to_string(*vv));
	// 	else if (auto* vv = get_if<string>(&v))  log(*vv);
	// }
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
	int runtimeerror() {
		logerr("runtime error, line " + to_string(lpos));
		throw runtime_error("syntaxerror");
	}
	int memoryerror() {
		logerr("memory error, line " + to_string(lpos));
		throw runtime_error("memoryerror");
	}

	// -- Run --
	void rfunc(const wfunc& fn) {
		lpos = fn.lpos;
		// for (auto& stmt : fn.block)
		// 	if      (auto* st = get_if<wprint>(&stmt))  rprint(*st);
		// 	else if (auto* st = get_if<wdim>(&stmt))    rdim(*st);
		// 	else if (auto* st = get_if<wlet>(&stmt))    rlet(*st);
		// 	else if (auto* st = get_if<wwhile>(&stmt))  rwhile(*st);
		// 	else    runtimeerror();  // warning: this will be wrong, but shouldn't be run
		rblock(fn.block);
	}

	void rblock(const wblock& block) {
		for (auto& stmt : block)
			if      (auto* st = get_if<wprint>(&stmt))  rprint(*st);
			else if (auto* st = get_if<wdim>(&stmt))    rdim(*st);
			else if (auto* st = get_if<wlet>(&stmt))    rlet(*st);
			else if (auto* st = get_if<wwhile>(&stmt))  rwhile(*st);
			else    runtimeerror();  // warning: this will be wrong, but shouldn't be run
	}

	void rprint(const wprint& pr) {
		lpos = pr.lpos;
		for (auto& arg : pr.list)
			if      (auto* s = get_if<string>(&arg))  log(*s);
			else if (auto* i = get_if<int>(&arg))     log(*i);
			else if (auto* v = get_if<wvar>(&arg)) {
				auto& mem = getmem(v->id);
				if      (auto* i = get_if<int>(&mem))     log(*i);
				else if (auto* s = get_if<string>(&mem))  log(*s);
			}
			else    runtimeerror();
		lognl();
	}

	void rdim(const wdim& dim) {
		lpos = dim.lpos;
		if (memory.count(dim.id))  memoryerror();
		memory[dim.id] = rexpr(dim.ex);
	}
	
	void rlet(const wlet& let) {
		lpos = let.lpos;
		if (!memory.count(let.id))  memoryerror();
		memory.at(let.id) = rexpr(let.ex);
	}

	void rwhile(const wwhile& wwl) {
		lpos = wwl.lpos;
		while (rexpri(wwl.ex))
			rblock(wwl.block);
	}

	// -- Expressions --
	Mem_T rexpr(const wexpr& ex) {
		if (auto* a = get_if<watom>(&ex)) {
			if      (auto* v = get_if<string>(a))  return *v;
			else if (auto* v = get_if<wvar>(a))    return getmem(v->id);
		}
		return rexpri(ex);
	}

	int rexpri(const wexpr& ex) {
		if (auto* a  = get_if<watom>(&ex))
			return atomi(*a);
		if (auto* op = get_if<wexprop>(&ex)) {
			int a = rexpri( op->ab.at(0) ),
				b = rexpri( op->ab.at(1) );
			// basic maths
			if      (op->op == "+" )  return a +  b;
			else if (op->op == "-" )  return a -  b;
			else if (op->op == "*" )  return a *  b;
			else if (op->op == "/" )  return a /  b;
			// comparisons
			else if (op->op == "<" )  return a <  b;
			else if (op->op == "<=")  return a <= b;
		}
		return runtimeerror();
	}

	int atomi(const watom& a) {
		if      (auto* v = get_if<int>(&a))   return *v;
		else if (auto* v = get_if<wvar>(&a))  return getmemi(v->id);
		return runtimeerror();
	}

	// -- Memory --
	Mem_T& getmem(const string& id) {
		if (!memory.count(id))  memoryerror();
		return memory[id];
	}
	int& getmemi(const string& id) {
		int* i = get_if<int>(&getmem(id));
		if (!i)  memoryerror();
		return *i;
	}
};
