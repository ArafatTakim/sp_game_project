#include <animal.h>
#include <raymath.h>
#include <cstdlib>

void Animal::update(float deltaTime) {
	// Update animation
	frameTimer += deltaTime;
	if (frameTimer >= animationSpeed) {
		frameTimer = 0.0f;
		currentFrame = (currentFrame + 1) % frameCount;
	}

	// Update movement timer
	moveTimer += deltaTime;
	if (moveTimer >= moveInterval) {
		moveTimer = 0.0f;
		changeDirection();
		moveInterval = 1.5f + (rand() % 30 / 10.0f); // 1.5-4.5 seconds
	}
}

void Animal::draw() {
	if (texture.id == 0) return;

	float frameWidth = texture.width / (float)frameCount;
	float frameHeight = (float)texture.height;

	Rectangle sourceRec = {
		currentFrame * frameWidth,
		0,
		frameWidth,
		frameHeight
	};

	Rectangle destRec = {
		pos.x,
		pos.y,
		1.0f,
		1.0f
	};

	DrawTexturePro(texture, sourceRec, destRec, { 0.5f, 0.5f }, 0.0f, WHITE);
}

void Animal::changeDirection() {
	velocity.x = (rand() % 2 == 0) ? 1.0f : -1.0f;
	velocity.x *= (0.5f + (rand() % 50 / 100.0f)); // 0.5-1.0 speed
}

void AnimalManager::addAnimal(Vector2 pos, AnimalType type, Texture2D texture) {
	Animal newAnimal;
	newAnimal.pos = pos;
	newAnimal.type = type;
	newAnimal.texture = texture;
	newAnimal.velocity = { 1.0f, 0 };
	newAnimal.frameCount = 4;
	newAnimal.animationSpeed = 0.1f;
	animals.push_back(newAnimal);
}

void AnimalManager::updateAll(float deltaTime) {
	for (auto& animal : animals) {
		animal.update(deltaTime);
		animal.pos.x += animal.velocity.x * deltaTime;
	}
}

void AnimalManager::drawAll() {
	for (auto& animal : animals) {
		animal.draw();
	}
}

void AnimalManager::clear() {
	animals.clear();
}
