#include <gameMap.h>


void GameMap::create(int x, int y) {
	mapData.resize(x * y);
	this->w = x;
	this->h = y;

	for (auto& e : mapData) e = {};
}

Block& GameMap::getBlock(int x, int y) {
	static Block airBlock = {};
	if (x < 0 || x >= w || y < 0 || y >= h) {
		return airBlock;
	}

	return mapData[x + w * y];
}