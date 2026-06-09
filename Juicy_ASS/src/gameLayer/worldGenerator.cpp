
#include <worldGenerator.h>
#include <FastNoiseSIMD.h>
#include <cmath>
#include <raymath.h>
#include <random>
#include <randomFuncs.h>
#include <structure.h>
#include <saveMap.h>

Biome Regular, Desert, Ice, SelectedBiome;

Structure buildStruct;


void buildInMap(GameMap& gameMap, int x, int  y, std::string path) {
	loadBlockDataFromFile(buildStruct.mapData, buildStruct.w, buildStruct.h, path.c_str());
	buildStruct.pasteIntoMap(gameMap, Vector2{ (float)(x), (float)(y - buildStruct.h) });
}

void structBottomFiller(GameMap& gameMap, int x, int y, int b){
	for (int i = x; i <= x + buildStruct.w; i++) {
		for (int j = y; gameMap.getBlock(i, j).type == Block::air; j++) {
			gameMap.getBlock(i, j).type = b;
		}
	}
}

void generateWorld(GameMap& gameMap, int seed) {

	//Biome Initializer
	Regular.dirtBlock.type  = Block::dirt;
	Regular.stoneBlock.type = Block::stone;
	Regular.topBlock.type = Block::grassBlock;

	Desert.dirtBlock.type  = Block::sand;
	Desert.stoneBlock.type = Block::sandStone;
	Desert.topBlock.type = Block::sand;

	Ice.dirtBlock.type  = Block::snow;
	Ice.stoneBlock.type = Block::ice;
	Ice.topBlock.type = Block::snow;



	int mapWidth = gameMap.w;
	int mapHeight = gameMap.h;

	std::ranlux24_base rng(seed);
	std::unique_ptr<FastNoiseSIMD> dirtNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());
	std::unique_ptr<FastNoiseSIMD> stoneNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());

	//setting up the dirt noise generator
	dirtNoiseGenerator->SetSeed(seed++);
	dirtNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::PerlinFractal);
	dirtNoiseGenerator->SetFractalOctaves(3);
	dirtNoiseGenerator->SetFrequency(0.01);

	//setting up the stone noise generator
	stoneNoiseGenerator->SetSeed(seed++);
	stoneNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::ValueFractal);
	stoneNoiseGenerator->SetFractalOctaves(2);
	stoneNoiseGenerator->SetFrequency(0.01);

	//dirt and stone noise set
	float* dirtNoise = FastNoiseSIMD::GetEmptySet(mapWidth);
	dirtNoiseGenerator->FillNoiseSet(dirtNoise, 0, 0, 0,  mapWidth, 1, 1);

	float* stoneNoise = FastNoiseSIMD::GetEmptySet(mapWidth);
	stoneNoiseGenerator->FillNoiseSet(stoneNoise, 0, 0, 0, mapWidth, 1, 1);


	int dirtStart = 200, stoneStart = 210;


	for (int i = 0; i < mapWidth; ) {

		int length = i + getRandomInt(rng, 70, 300);
		length = Clamp(length, 0, mapWidth);

		Block dirt, stone, top;
		dirt.type = Regular.dirtBlock.type;
		stone.type = Regular.stoneBlock.type;
		top.type = Regular.topBlock.type;

		float prob = getRandomFloat(rng, 0.f, 1.f);
		if (prob <= 0.6) {
			dirt.type = Regular.dirtBlock.type;
			stone.type = Regular.stoneBlock.type;
			top.type = Regular.topBlock.type;
		}

		else if (prob <= 0.8) {
			dirt.type = Desert.dirtBlock.type;
			stone.type = Desert.stoneBlock.type;
			top.type = Desert.topBlock.type;
		}
		else if(prob <= 0.9) {
			dirt.type = Ice.dirtBlock.type;
			stone.type = Ice.stoneBlock.type;
			top.type = Ice.topBlock.type;
		}

		for (int x = i; x < length; x++) {

			int dirtHeight = dirtStart + 30 * dirtNoise[x];
			int stoneHeight = stoneStart + 100 * stoneNoise[x] + (stoneNoise[x] > 0 ? (-70 * stoneNoise[x]) : 0);

			for (int y = 0; y < mapHeight; y++) {

				int biomeStoneMaxHeight = stoneHeight + getRandomInt(rng, 20, 50);
				auto& b = gameMap.getBlock(x, y);

				if (y >= biomeStoneMaxHeight) {
					b.type = Block::stone;
					continue;
				}
				if (y < dirtHeight && y < stoneHeight) {
					b.type = Block::air;
					continue;
				}

				if (y >= stoneHeight) {
					float oreChance = getRandomFloat(rng, 0.f, 1.f);
					if (oreChance <= 0.008789 && y >= stoneStart + 40) {
						b.type = Block::gold;
					}
					else if (oreChance <= 0.015 && y >= stoneStart+15) {
						b.type = Block::iron;
					}
					else if (oreChance <= 0.025) {
						b.type = Block::copper;
					}
						
					else
						b.type = stone.type;
				}

				else {
					if (y == dirtHeight) b.type = top.type;
					else
						b.type = dirt.type;
				}


			}

		}

		i = length;
	}


	for (int i = 0; i < 50; i++) {
		int startX = getRandomInt(rng, 0, mapWidth);
		int startY = getRandomInt(rng, 200, mapHeight - 300);

		int length = getRandomInt(rng, 500, 1500);

		for (int j = 0; j < length; j++) {
			int x = getRandomInt(rng, -1, 1);
			int y = getRandomInt(rng, -1, 1);
			startX += x;
			startY += y;
			startX = Clamp(startX, 0, gameMap.w - 1);
			startY = Clamp(startY, 0, gameMap.h - 1);
			float r = getRandomFloat(rng, 0, 1);
			if (r <= 0.0034 && startY > 220) {
				gameMap.getBlock(startX, startY).type = Block::goldBlock;
			}
			else if (r <= 0.0067 && startY > 220) {
				gameMap.getBlock(startX, startY).type = Block::ironBlock;
			}
			else
				gameMap.getBlock(startX, startY).type = Block::air;
		}
		if (startY > 230) {
			float prob = getRandomFloat(rng, 0.f, 1.f);
			if(prob <= 0.5)
				buildInMap(gameMap, startX, startY, RESOURCES_PATH "structures/mineshraft.bin");
			else if(prob <= 0.83)
				buildInMap(gameMap, startX, startY, RESOURCES_PATH "structures/cave_maze1.bin");
		}
	}

	//Tree Building
	for (int x = 1; x < mapWidth; x++) {
		for (int y = 0; y < mapHeight; y++) {
			std::string path = RESOURCES_PATH "structures/";
			if (gameMap.getBlock(x, y).type == Block::grassBlock) {
				float r = getRandomFloat(rng, 0, 1.f);
				if (r <= 0.005) {
					path += "tree_rare.bin";
					buildInMap(gameMap, x - 1, y, path);
				}
				else if (r <= 0.05) {
					path += "tree1.bin";
					buildInMap(gameMap, x - 1, y, path);
				}
				else if (r <= 0.067) {
					path += "tree2.bin";
					buildInMap(gameMap, x - 1, y, path);
				}
			}
		}
	}


	//biome structure building
	//1.Regukar Biome:
	int prevX = -1;
	for (int x = 0; x < mapWidth; x++) {
		for (int y = 0; y < mapHeight; y++) {
			std::string path = RESOURCES_PATH "structures/";
			auto b = gameMap.getBlock(x, y);

			if (b.type == Block::grassBlock) {
				float prob = getRandomFloat(rng, 0.f, 1.f);
				if (prob <= 0.00085) {
					path += "house_rare.bin";
					buildInMap(gameMap, x, y, path);
					structBottomFiller(gameMap, x, y, 8);
					x += buildStruct.w;
				}
				else if (prob <= 0.0083) {
					path += "house_one.bin";
					buildInMap(gameMap, x, y, path);
					structBottomFiller(gameMap, x, y, 8);
					x += buildStruct.w;
				}
				else if ((prob <= 0.012)) {
					path += "house_two.bin";
					buildInMap(gameMap, x, y, path);
					structBottomFiller(gameMap, x, y, 8);
					x += buildStruct.w;
				}
			}

			else if (b.type == Block::sand) {
				float prob  = getRandomFloat(rng, 0.f, 1.f);

				if (prob <= 0.012 && gameMap.getBlock(x, y-1).type == Block::air) {
					path += "sand_castle.bin";
					buildInMap(gameMap, x, y, path);
					structBottomFiller(gameMap, x, y, Block::sandStone);
					x += buildStruct.w;
				}

				else if (prob <= 0.005) {
					path += "pyramid.bin";
					buildInMap(gameMap, x, y, path);
					x += buildStruct.w;
				}
			}

			else if (b.type == Block::snow) {
				float prob = getRandomFloat(rng, 0.f, 1.f);
				if (prob <= 0.0082 && gameMap.getBlock(x, y - 1).type == Block::air) {
					path += "igloo.bin";
					buildInMap(gameMap, x, y, path);
					structBottomFiller(gameMap, x, y, Block::snow);
					x += buildStruct.w;
				}
			}

		}
		
	}


	FastNoiseSIMD::FreeNoiseSet(dirtNoise);
	FastNoiseSIMD::FreeNoiseSet(stoneNoise);

}