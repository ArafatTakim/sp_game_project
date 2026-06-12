#pragma once
#include <raylib.h>
#include <vector>
#include <string>

enum AnimalType {
	SLIME = 0,
	BLUE_SLIME,
	SMALL_BLUE_SLIME,
	ICE_SLIME,
	MUMMY,
	ZOMBIE,
	ZOMBIE_ESKIMO,
	EVIL_EYE,
	ANIMAL_COUNT
};

struct Animal {
	Vector2 pos;
	AnimalType type;
	Texture2D texture;
	float frameTimer = 0.0f;
	int currentFrame = 0;
	int frameCount = 4; // Number of animation frames
	float animationSpeed = 0.1f;
	Vector2 velocity = { 0, 0 };
	float moveTimer = 0.0f;
	float moveInterval = 2.0f;

	void update(float deltaTime);
	void draw();
	void changeDirection();
};

struct AnimalManager {
	std::vector<Animal> animals;

	void addAnimal(Vector2 pos, AnimalType type, Texture2D texture);
	void updateAll(float deltaTime);
	void drawAll();
	void clear();
};
