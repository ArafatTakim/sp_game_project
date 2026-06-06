#include <raylib.h>
#include "gameMain.h"
#include <assetManager.h> 
#include <gameMap.h>
#include <helper.h>
#include <iostream>
#include <raymath.h>
#include <cmath>
#include <worldGenerate.h>
#include <imgui.h>
#include <structure.h>
#include <saveMap.h>



struct Player {
    Vector2 position;
    Vector2 velocity;
    float speed;
    Texture2D texture;
};
struct GameData {
	GameMap gameMap;
	Camera2D camera;

	int creativeSelectedBlock = Block::dirt;

	Vector2 selectionStart = {0,0};
	Vector2 selectionEnd = {0,0};

	Structure copyStructure;
	char saveName[100] = {};
	Player player;

}gameData;

AssetManager assetManager;

bool showImgui = false;


bool initGame() {
    assetManager.loadAll();
	generateWorld(gameData.gameMap);

	gameData.camera.target = { 300,177 };
	gameData.camera.rotation = 0.0f;
	gameData.camera.zoom = 50.0f;
	gameData.player.position = { 300, 100 };
    gameData.player.speed = 5.0f;
    gameData.player.texture = LoadTexture("resources/player.png");

	return true;
}

bool updateGame() {

	float deltaTime = GetFrameTime();
	if (deltaTime > 1.f / 5) deltaTime = 1 / 5.f;

	gameData.camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

	

	ClearBackground({ 75,75,150,255 });

	if (IsKeyPressed(KEY_C)) showImgui = !showImgui;

	static float CAMERA_SPEED = 10;
	if (IsKeyDown(KEY_LEFT)) gameData.camera.target.x -= CAMERA_SPEED * GetFrameTime();
	if (IsKeyDown(KEY_RIGHT)) gameData.camera.target.x += CAMERA_SPEED * GetFrameTime();
	if (IsKeyDown(KEY_UP)) gameData.camera.target.y -= CAMERA_SPEED * GetFrameTime();
	if (IsKeyDown(KEY_DOWN)) gameData.camera.target.y += CAMERA_SPEED * GetFrameTime();
	Vector2 dir = {0, 0};

    if (IsKeyDown(KEY_A)) dir.x -= 1;
    if (IsKeyDown(KEY_D)) dir.x += 1;
    if (IsKeyDown(KEY_W)) dir.y -= 1;
    if (IsKeyDown(KEY_S)) dir.y += 1;

    gameData.player.position.x += dir.x * gameData.player.speed;
    gameData.player.position.y += dir.y * gameData.player.speed;

	gameData.camera.target = gameData.player.position;

	Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);
	int blockX = (int)floor(worldPos.x);
	int blockY = (int)floor(worldPos.y);
	std::cout << blockX << " " << blockY << "\n";


	if (showImgui) {
		if (IsKeyPressed(KEY_ONE)) {
			gameData.selectionStart = Vector2{ (float)blockX, (float)blockY };
		}
		if (IsKeyPressed(KEY_TWO)) {
			gameData.selectionEnd = Vector2{ (float)blockX, (float)blockY };
		}

		if (IsKeyPressed(KEY_THREE)) {
			gameData.copyStructure.pasteIntoMap(gameData.gameMap, Vector2{ (float)blockX, (float)blockY });
		}


		if (gameData.selectionStart.x > gameData.selectionEnd.x) {
			std::swap(gameData.selectionStart.x, gameData.selectionEnd.x);
		}

		if (gameData.selectionStart.y > gameData.selectionEnd.y) {
			std::swap(gameData.selectionStart.y, gameData.selectionEnd.y);
		}


	}

	if (!showImgui) {
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			auto& b = gameData.gameMap.getBlock(blockX, blockY);
			b.type = Block::air;
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			auto& b = gameData.gameMap.getBlock(blockX, blockY);
			b.type = gameData.creativeSelectedBlock;
		}
	}



	BeginMode2D(gameData.camera);

	Vector2 topLeftView = GetScreenToWorld2D({ 0,0 }, gameData.camera);
	Vector2 bottomRightView = GetScreenToWorld2D({ (float)GetScreenWidth(), (float)GetScreenHeight() }, gameData.camera);

	int startXView = floorf(topLeftView.x - 1);
	int endXView = ceilf(bottomRightView.x - 1);
	int startYView = floorf(topLeftView.y - 1);
	int endYView = ceilf(bottomRightView.y - 1);


	startXView = Clamp(startXView, 0, gameData.gameMap.w);
	endXView = Clamp(endXView, 0, gameData.gameMap.w);
	startYView = Clamp(startYView, 0, gameData.gameMap.h);
	endYView = Clamp(endYView, 0, gameData.gameMap.h);


	for (int y = startYView; y <= endYView; y++) {
		for (int x = startXView; x <= endXView; x++) {

			auto& b = gameData.gameMap.getBlock(x, y);
			if (b.type == Block::air) {	
				continue;
			}

			
			DrawTexturePro(
				assetManager.textures,
				getTextureAtlas(b.type, 0, 32, 32),
				{ (float)x, (float)y, 1, 1 },
				{ 0,0 },
				0.0f,
				WHITE
			);
		}
	}

	DrawTexturePro(
		assetManager.frame,
		{0,0,(float)assetManager.frame.width, (float)assetManager.frame.height},
		{ (float)blockX, (float)blockY, 1,1 },
		{ 0,0 },
		0.f,
		WHITE
	);
	DrawTexture(
    gameData.player.texture,
    gameData.player.position.x,
    gameData.player.position.y,
    WHITE
    );


	if (showImgui) {
		Rectangle rect;
		rect.x = gameData.selectionStart.x;
		rect.y = gameData.selectionStart.y;
		rect.width = gameData.selectionEnd.x - gameData.selectionStart.x + 1;
		rect.height = gameData.selectionEnd.y - gameData.selectionStart.y + 1;

		DrawRectangleLinesEx(rect, 0.1, { 20,101,250,255 });
	}


	EndMode2D();

	


	if (showImgui) {
		ImGui::Begin("Game controll");

		ImGui::SliderFloat("Camera zoom:", &gameData.camera.zoom, 1, 150);
		ImGui::SliderFloat("Camera speed:", &CAMERA_SPEED, 5, 100);

		if (ImGui::Button("Copy")) {
			gameData.copyStructure.copyFromMap(gameData.gameMap, gameData.selectionStart, gameData.selectionEnd);
		}
		ImGui::InputText("File name", gameData.saveName, sizeof(gameData.saveName));

		if (ImGui::Button("Save to File")) {
			std::string path = RESOURCES_PATH "structures/";
			path += gameData.saveName;
			path += ".bin";

			saveBlockDataToFile(gameData.copyStructure.mapData, gameData.copyStructure.w, gameData.copyStructure.h, path.c_str());
		}

		if (ImGui::Button("Load From File")) {
			std::string path = RESOURCES_PATH "structures/";
			path += gameData.saveName;
			path += ".bin";

			loadBlockDataFromFile(gameData.copyStructure.mapData, gameData.copyStructure.w, gameData.copyStructure.h, path.c_str());
		}

		ImGui::Separator();

		for (int i = 0; i < Block::BLOCKS_COUNT; i++) {
			auto atlas = getTextureAtlas(i, 0, 32, 32);

			atlas.x /= assetManager.textures.width;
			atlas.width /= assetManager.textures.width;
			atlas.y /= assetManager.textures.height;
			atlas.height /= assetManager.textures.height;

			ImGui::PushID(i);

			ImTextureID tex = (ImTextureID)(intptr_t)assetManager.textures.id;

			if (ImGui::ImageButton(tex,
				{ 35,35 }, { atlas.x, atlas.y },
				{ atlas.x + atlas.width, atlas.y + atlas.height })) {
				gameData.creativeSelectedBlock = i;
			}
			ImGui::PopID();

			if (i % 10 != 0) {
				ImGui::SameLine();
			}
		}


		ImGui::End();
	}

	DrawFPS(10, 10);
	return true;
}


void closeGame() {
    
}