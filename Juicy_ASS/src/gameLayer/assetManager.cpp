#pragma once
#include <assetManager.h>

void AssetManager::loadAll() {
	textures = LoadTexture(RESOURCES_PATH "texturesWithBackgroundVersion.png");
	forestBg = LoadTexture(RESOURCES_PATH "forestBG.png");
	texturesbg = LoadTexture(RESOURCES_PATH "texturesWithBackgroundVersion.png");
	frame = LoadTexture(RESOURCES_PATH "frame.png");
	player = LoadTexture(RESOURCES_PATH "player.png");
	blueSlime = LoadTexture(RESOURCES_PATH "blueslime.png");
}
