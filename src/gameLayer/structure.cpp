#include <structure.h>

void Structure::create(int w, int h) {
	mapData.resize(w * h);

	this->w = w;
	this->h = h;

	for (auto &e : mapData) e = {};
}


Block& Structure::getBlock(int x, int y) {

	static Block airBlock{};

	if (x < 0 || y < 0 || x >= w || y >= h)
		return airBlock;

	return mapData[x + y * w];
}


void Structure::copyFromMap(GameMap& map, Vector2 start, Vector2 end) {

	Vector2 size = { end.x - start.x + 1, end.y - start.y + 1 };
	create(size.x, size.y);

	for (int y = 0; y < size.y; y++) {
		for (int x = 0; x < size.x; x++) {
			getBlock(x, y) = map.getBlock(start.x + x, start.y + y);
		}
	}
}


void Structure::pasteIntoMap(GameMap &map, Vector2 start) {
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			auto& b = map.getBlock(x + start.x, y + start.y);
			auto& temp = getBlock(x, y);
			if (temp.type != Block::air) b = temp;
	
		}
	}
}