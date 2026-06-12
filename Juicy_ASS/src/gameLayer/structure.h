#pragma once
#include<gameMap.h>
#include <raylib.h>
#include <block.h>
#include <vector>
#include <raymath.h>

struct Structure {
	int h = 0;
	int w = 0;

	std::vector<Block> mapData;

	void create(int x, int y);

	Block& getBlock(int x, int y);

	void copyFromMap(GameMap& map, Vector2 start, Vector2 end);

	void pasteIntoMap(GameMap& map, Vector2 start);
};