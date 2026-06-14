#pragma once

#include <raylib.h>

struct AssetManager {

	Texture2D textures;
	Texture2D forestBg;
	Texture2D texturesbg;
	Texture2D frame;
	Texture2D player;
    Texture2D blueSlime;


	void loadAll();
};
