#include <saveMap.h>


bool saveBlockDataToFile(std::vector<Block> block, int w, int h, const char* filename) {

	std::ofstream f(filename, std::ios::binary);

	if (!f.is_open()) return false;
	if (block.size() != w * h) return false;

	f.write((const char*)&w, sizeof(w));
	f.write((const char*)&h, sizeof(h));

	f.write((const char*)block.data(), sizeof(Block) * block.size());

	f.close();

	return true;

}


bool loadBlockDataFromFile(std::vector<Block>& blocks, int& w, int& h, const char* filename) {

	blocks.clear();
	w = 0;
	h = 0;

	std::ifstream f(filename, std::ios::binary);

	if (!f.is_open()) return false;

	f.read((char*)&w, sizeof(w));
	f.read((char*)&h, sizeof(h));

	if (!f || w <= 0 || h <= 0) {
		f.close();
		return false;
	}

	if (w > 10000 || h > 10000) {
		f.close();
		return false;
	}

	size_t blockCount = w * h;
	blocks.resize(blockCount);

	f.read((char*)blocks.data(), sizeof(Block) * blocks.size());

	if (!f) {

		blocks.clear();
		w = 0;
		h = 0;
		f.close();
		return false;
	}

	for (size_t i = 0; i < blocks.size(); i++) {
		if (blocks[i].type >= Block::BLOCKS_COUNT) {
			blocks[i].type = Block::air;
		}
	}

	f.close();
	return true;

}