#pragma once

extern WBParser wbparser;

struct WBRuntime2 : WBParserBase {
	int start() {
		if (!wbparser.functions.count("main"))
			logerr("no function found: main");
		return rfunc(wbparser.functions.at("main"));
	}

	int rfunc(const wfunc& fn) {
		for (auto& stmt : fn.block)
			if (const wprint* pr = get_if<wprint>(&stmt))  rprint(*pr);
		return true;
	}

	int rprint(const wprint& pr) {
		printf("%s\n", pr.s.c_str());
		return true;
	}
};
