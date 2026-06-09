#pragma once

#include <raylib.h>
#include <block.h>
#include <vector>

struct GameMap {
	int h = 0;
	int w = 0;

	std::vector<Block> mapData;

	void create(int x, int y);

	Block& getBlock(int x, int y);
};