#pragma once
#include "raylib.h"
#include <string>
#include <map>
using namespace std;

struct Assets {
	static inline map<string, Texture> assets;
	static inline Texture defaulttex = {0};

	static int loadimage(const string& alias, const string& fname) {
		if (assets.count(alias))
			return fprintf(stderr, "Asset already exists: %s\n", alias.c_str()), 1;
		Texture tex = LoadTexture(fname.c_str());
		if (!IsTextureValid(tex))
			return fprintf(stderr, "Error loading asset: %s (%s)\n", alias.c_str(), fname.c_str()), 1;
		assets[alias] = tex;
		return 0;
	}

	static int unload(const string& alias) {
		if (assets.count(alias))  return 0;
		UnloadTexture(assets.at(alias));
		assets.erase(alias);
		return 0;
	}

	static Texture& gettexture(const string& alias) {
		try {
			return assets.at(alias);
		} catch(out_of_range& e) {
			fprintf(stderr, "Missing texture: %s\n", alias.c_str());
			return defaulttex;
		}
	}
};
