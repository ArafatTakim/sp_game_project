#pragma once
#include <raylib.h>
#include <block.h>
#include <gameMap.h>

typedef struct Biome {
	Block topBlock = {};
	Block dirtBlock = {};
	Block stoneBlock;
};


void generateWorld(GameMap& gameMap, int seed);