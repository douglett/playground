#pragma once
#include "raylib.h"
#include <string>
#include <map>
using namespace std;

struct Assets {
	static inline map<string, Texture> assets;
	static inline map<string, bool> missing;
	static inline Texture defaulttex = {0};

	static void destroy() {
		while (assets.size())
			unload(assets.begin()->first);
	}

	static void unload(const string& alias) {
		if (!assets.count(alias))  return;
		UnloadTexture(assets.at(alias));
		assets.erase(alias);
	}

	static int loadtexture(const string& alias, const string& fname) {
		if (assets.count(alias))
			return fprintf(stderr, "Asset already exists: %s\n", alias.c_str()), 1;
		Texture tex = LoadTexture(fname.c_str());
		if (!IsTextureValid(tex))
			return fprintf(stderr, "Error loading asset: %s (%s)\n", alias.c_str(), fname.c_str()), 1;
		assets[alias] = tex;
		return 0;
	}

	static Texture& gettexture(const string& alias) {
		if (assets.count(alias))
			return assets.at(alias);
		else if (!missing.count(alias)) {
			fprintf(stderr, "Missing texture: %s\n", alias.c_str());
			missing[alias] = true;
		}
		return defaulttex;
	}
};
