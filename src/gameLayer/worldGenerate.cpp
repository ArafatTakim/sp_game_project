#include <memory>
#include <worldGenerate.h>
#include <blocks.h>
#include <random>
#include <randomStuff.h>
#include <FastNoiseSIMD.h>
#include <cmath>
#include <raymath.h>
#include <buildStructs.h>




void generateWorld(GameMap& gameMap, int seed) {

	int h = 500;
	int w = 900;

	gameMap.create(w, h);

	std::ranlux24_base rng(seed);

	int desertStart = getRandomInt(rng, 10, w - 210);
	int desertEnd = desertStart + getRandomInt(rng, 50, 200);
	if (desertEnd >= w) desertEnd = w - 1;

	int desertMid = (desertStart + desertEnd) / 2;

	int D = (desertEnd - desertStart) / 2;


	std::unique_ptr<FastNoiseSIMD> dirtNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());
	std::unique_ptr<FastNoiseSIMD> stoneNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());
	std::unique_ptr<FastNoiseSIMD> caveNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());

	dirtNoiseGenerator->SetSeed(seed++);
	stoneNoiseGenerator->SetSeed(seed++);
	caveNoiseGenerator->SetSeed(seed++);

	dirtNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::PerlinFractal);
	dirtNoiseGenerator->SetFractalOctaves(3);
	dirtNoiseGenerator->SetFrequency(0.01);
	
	stoneNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::PerlinFractal);
	stoneNoiseGenerator->SetFractalOctaves(4);
	stoneNoiseGenerator->SetFrequency(0.04);

	caveNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
	caveNoiseGenerator->SetFractalOctaves(6);
	caveNoiseGenerator->SetFrequency(0.02);


	float* dirtNoise = FastNoiseSIMD::GetEmptySet(w);
	float* stoneNoise = FastNoiseSIMD::GetEmptySet(w);
	float* caveNoise = FastNoiseSIMD::GetEmptySet(w * h);
	dirtNoiseGenerator->FillNoiseSet(dirtNoise, 0, 0, 0, w, 1, 1);
	stoneNoiseGenerator->FillNoiseSet(stoneNoise, 0, 0, 0, w, 1, 1);
	caveNoiseGenerator->FillNoiseSet(caveNoise, 0, 0, 0, h, w, 1);

	for (int i = 0; i < w; i++) {
		dirtNoise[i] = (dirtNoise[i] + 1) / 2;
		stoneNoise[i] = (stoneNoise[i] + 1) / 2;
	}
	for (int i = 0; i < w * h; i++) {
		caveNoise[i] = (caveNoise[i] + 1) / 2;
	}

	auto getCaveNoise = [&](int x, int y) {
		return caveNoise[x + y * w];
	};


	int dirtStart = 200;
	int stoneStart = 300;


	int tempX = 0;

	for (int x = 0; x < w; x++) {
		bool isDesert = false;
		int dirtHeight = dirtStart - 40 * dirtNoise[x];
		int stoneHeight = stoneStart - 200 * stoneNoise[x];
		Block b;
		float maxDesertHeight = -(100.0f / (D * D)) * (x - desertStart) * (x - desertEnd) + dirtStart;

		// Tree Building
		bool isTree = getRandomChance(rng, 0.067);

		for (int y = 0; y < h; y++) {
			
			int dirtType = Block::dirt;
			int grassBlockType = Block::grassBlock;
			int stoneType = Block::stone;
			
			if (x >= desertStart && x <= desertEnd && y <=maxDesertHeight) {
				dirtType = Block::sand;
				stoneType = Block::sandStone;
				grassBlockType = Block::sand;
				isDesert = true;
			}

			if (y < dirtHeight) continue;

			if (y == dirtHeight) {
				
				if (isTree && abs(x-tempX) > 6 && !isDesert) {
					buildTree(gameMap, x+1, y);
					tempX = x;
				}

				b.type = grassBlockType;

			}
			else {
				if (y > dirtHeight) {
					b.type = dirtType;
				}

				if (y >= stoneHeight) {
					b.type = stoneType;
				}
			}
			
			//if (getCaveNoise(x, y) < 0.3) {
				//b.type = Block::air;
			//}

			gameMap.getBlock(x, y) = b;
		}
	}

	for (int i = 0; i < 50; i++) {
		int startX = getRandomInt(rng, 0, w);
		int startY = getRandomInt(rng, 200, h-200);

		int length = getRandomInt(rng, 500, 1500);

		for (int j = 0; j < length; j++) {
			int x = getRandomInt(rng, -1, 1);
			int y = getRandomInt(rng, -1, 1);
			startX += x;
			startY += y;
			startX = Clamp(startX, 0, gameMap.w - 1);
			startY = Clamp(startY, 0, gameMap.h - 1);
			gameMap.getBlock(startX, startY).type = Block::air;
		}
	}



	FastNoiseSIMD::FreeNoiseSet(dirtNoise);
	FastNoiseSIMD::FreeNoiseSet(stoneNoise);
}
