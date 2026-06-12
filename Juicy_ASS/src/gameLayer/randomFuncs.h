#pragma once
#include <random>

float getRandomFloat(std::ranlux24_base &rng, float min, float max);
bool getRandomChance(std::ranlux24_base &rng, float p);
int getRandomInt(std::ranlux24_base &rng, int min, int max);