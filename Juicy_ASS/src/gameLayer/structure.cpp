#include <structure.h>
#include <vector>

void Structure::create(int x, int y) {
	mapData.resize(x * y);
	this->w = x;
	this->h = y;

	for (auto& e : mapData) e = {};
}

Block& Structure::getBlock(int x, int y) {
	static Block airBlock = {};
	if (x < 0 || x >= w || y < 0 || y >= h) {
		return airBlock;
	}

	return mapData[x + w * y];
}


void Structure::copyFromMap(GameMap& map, Vector2 start, Vector2 end) {
	Vector2 size = { end.x - start.x + 1, end.y - start.y + 1 };
	create(size.x, size.y);

	for (int x = 0; x < size.x; x++) {
		for (int y = 0; y < size.y; y++) {
			getBlock(x, y) = map.getBlock(start.x + x, start.y + y);
		}
	}
}


void Structure::pasteIntoMap(GameMap& map, Vector2 start) {
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			auto& b = map.getBlock(x + start.x, y + start.y);
			auto& temp = getBlock(x, y);
			if (temp.type != Block::air) b = temp;

		}
	}
}
