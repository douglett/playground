#pragma once

extern WBParser wbparser;

struct WBRuntime2 : WBParserBase {
	int start() {
		if (!wbparser.functions.count("main"))
			logerr("no function found: main");
		return rfunc(wbparser.functions.at("main"));
	}

	int runtimeerror(int lpos) {
		logerr("runtime error, line " + to_string(lpos));
		throw runtime_error("syntaxerror");
	}

	int rfunc(const wfunc& fn) {
		for (auto& stmt : fn.block)
			if (const wprint* pr = get_if<wprint>(&stmt))  rprint(*pr);
		return true;
	}

	int rprint(const wprint& pr) {
		for (auto& v : pr.list)
			if      (auto* s = get_if<string>(&v))  printf("%s ", s->c_str());
			else if (auto* i = get_if<int>(&v))     printf("%i ", *i);
			else    runtimeerror(pr.lpos);
		printf("\n");
		return true;
	}
};
