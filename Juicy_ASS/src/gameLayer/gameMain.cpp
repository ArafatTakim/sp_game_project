#include <gameMain.h>
#include <raylib.h>
#include <raymath.h>
#include <assetManager.h>
#include <gameMap.h>
#include <worldGenerator.h>
#include <imgui.h>
#include <rlImGui.h>
#include <structure.h>
#include <saveMap.h>
#include <physics.h>
#include <blueslime.h>

struct GameData {
	Camera2D camera;
	GameMap gameMap = {};

	int w = 5000;
	int h = 600;

	int selectedBlockType = Block::dirt;

	Vector2 selectionStart = { 0,0 };
	Vector2 selectionEnd = { 0,0 };
	
	Structure copyStructure;

	char saveName[100] = {};

	PhysicalEntity player;
	BlueSlime slime;

}gameData;

AssetManager assetManager;
bool showimgui = false;


bool initGame() {
	gameData.camera.target = { 100,120 };
	gameData.camera.rotation = 0.0f;
	gameData.camera.zoom = 100;
	//gameData.camera.offset = { GetScreenWidth() / 2.f, GetScreenHeight() / 2.f + 50.f };

	assetManager.loadAll();

	gameData.gameMap.create(gameData.w, gameData.h);
	generateWorld(gameData.gameMap, 69);

	gameData.player.transform = { {100.7f,200.5f}, 0.8f, 1.8f };
	gameData.slime.body.transform =
   {
    {120.f, 200.f},
    0.8f,
    0.8f
   };

	return true;
}


bool updateGame() {

	static float ballX = 100, ballY = 30;
	static float cameraZoom = 50;
	static int cameraSpeed = 10.f;
	static float jump = 10.5;
	static float up = 3.f;

	float deltaTime = GetFrameTime();
	deltaTime = Clamp(deltaTime, 0.f, 1 / 5.f);
	gameData.camera.offset = { GetScreenWidth() / 2.f, GetScreenHeight() / 2.f };
	BeginMode2D(gameData.camera);

	if (IsKeyPressed(KEY_F10)) showimgui = !showimgui;

	if (IsKeyDown(KEY_W)) gameData.player.transform.pos.y -= cameraSpeed * deltaTime;
	if (IsKeyDown(KEY_S)) gameData.player.transform.pos.y += cameraSpeed * deltaTime;
	if (IsKeyDown(KEY_A)) gameData.player.transform.pos.x -= cameraSpeed * deltaTime;
	if (IsKeyDown(KEY_D)) gameData.player.transform.pos.x += cameraSpeed * deltaTime;
	if (IsKeyPressed(KEY_SPACE)) gameData.player.velocity.y =  -jump;

	if (IsKeyPressed(KEY_Z)) cameraZoom += 5;
	if (IsKeyPressed(KEY_X)) cameraZoom -= 5;
	if (IsKeyPressed(KEY_UP)) cameraSpeed += 5;
	if (IsKeyPressed(KEY_DOWN)) cameraSpeed -= 5;

	cameraSpeed = Clamp(cameraSpeed, 1, 100);
	cameraZoom = Clamp(cameraZoom, 1, 100);


	gameData.camera.zoom = cameraZoom;

	//updating player pos and camera pos ->
	gameData.player.updateForces(deltaTime);
	gameData.player.checkCollisionOnce(gameData.gameMap, gameData.player.transform.pos);
	gameData.camera.target = {gameData.player.transform.pos.x ,gameData.player.transform.pos.y - up};
	gameData.player.updateFinal();


	Vector2 topLeftView = GetScreenToWorld2D({ 0,0 }, gameData.camera);
	Vector2 bottomRightView = GetScreenToWorld2D({ (float)GetScreenWidth(), (float)GetScreenHeight() }, gameData.camera);

	int startViewX = floorf(topLeftView.x - 1);
	int endViewX = ceilf(bottomRightView.x - 1);
	int startViewY = floorf(topLeftView.y - 1);
	int endViewY = ceilf(bottomRightView.y - 1);

	startViewX = Clamp(startViewX, 0, gameData.w );
	endViewX = Clamp(endViewX, 0, gameData.w );
	startViewY = Clamp(startViewY, 0, gameData.h );
	endViewY = Clamp(endViewY, 0, gameData.h );
	
	Vector2 cursorPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);
	int blockX = cursorPos.x;
	int blockY = cursorPos.y;
	gameData.slime.UpdateBlueSlime(
    gameData.gameMap,
    deltaTime
    );


	if (!showimgui) {
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			gameData.gameMap.getBlock(blockX, blockY).type = Block::air;
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			gameData.gameMap.getBlock(blockX, blockY).type = gameData.selectedBlockType;
		}
	}

	if (showimgui) {
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



	
	ClearBackground(WHITE);

	//background loading 

	// After loading the texture
	SetTextureWrap(assetManager.forestBg, TEXTURE_WRAP_REPEAT);
	float parallaxFactor = 0.5f;
	static float down = 10;
	static float zoom = 1.2f; // > 1.0 = zoom out, < 1.0 = zoom in

	float bgW = GetScreenWidth() * zoom;
	float bgH = GetScreenHeight() * zoom;

	float bgX = gameData.camera.target.x * parallaxFactor - bgW * 0.5f;
	float bgY = gameData.camera.target.y * parallaxFactor - bgH * 0.5f - down;

	DrawTexturePro(
		assetManager.forestBg,
		{
			bgX,
			bgY,
			bgW,
			bgH
		},
	{
		gameData.camera.target.x - GetScreenWidth() * 0.5f,
		gameData.camera.target.y - GetScreenHeight() * 0.5f,
		(float)GetScreenWidth(),
		(float)GetScreenHeight()
	},
		{ 0, 0 },
		0.0f,
		WHITE
	);


	
	for (int x = startViewX; x <= endViewX; x++) {
		for (int y = startViewY; y <= endViewY; y++) {

			auto& b = gameData.gameMap.getBlock(x, y);
			
			if (b.type == Block::air) {
				continue;
			}

			Rectangle rec = { (float)b.type * 32.0f, 0.0f, 32.0f,32.0f };

			DrawTexturePro(
				assetManager.textures,
				rec,
				{ (float)x, (float)y, 1, 1 },
				{ 0.f,0.f },
				0.f,
				WHITE
			);
		}
	}

	if (showimgui) {
		Rectangle rect;
		rect.x = gameData.selectionStart.x;
		rect.y = gameData.selectionStart.y;
		rect.width = gameData.selectionEnd.x - gameData.selectionStart.x + 1;
		rect.height = gameData.selectionEnd.y - gameData.selectionStart.y + 1;

		DrawRectangleLinesEx(rect, 0.1, { 20,101,250,255 });
	}

	DrawTexturePro(
		assetManager.frame,
		{ 0,0,(float)assetManager.frame.width, (float)assetManager.frame.height },
		{ (float)blockX, (float)blockY, 1,1 },
		{ 0,0 },
		0.f,
		WHITE
	);

	DrawTexturePro(
		assetManager.player,
		{0,0,(float)assetManager.player.width, (float)assetManager.player.height},
		{gameData.player.transform.getTopLeft().x, gameData.player.transform.getTopLeft().y, 0.9f, 1.8f},
		{0,0},
		0.f,
		WHITE
	);
	DrawTexturePro(
    assetManager.blueSlime,
    {
        0,
        0,
        48,
        50
    },
    {
        gameData.slime.body.transform.getTopLeft().x,
        gameData.slime.body.transform.getTopLeft().y,
        0.8f,
        0.8f
    },
    {0,0},
    0,
    WHITE
    );

	DrawRectangleLinesEx(gameData.player.transform.getAABB(), 0.1, BLANK); //player aabb


	EndMode2D();

	if (showimgui) {

		ImGui::Begin("test");
		ImGui::SliderFloat("up", &up, 0, 10);
		ImGui::SliderFloat("jump", &jump, 10, 50);
		ImGui::SliderFloat("up/down", &down, 10.f, 100.f);
		ImGui::SliderFloat("zoom in/out", &zoom, 0, 5);
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

		for (int i = 1; i < Block::BLOCKS_COUNT; i++) {
			Rectangle atlas{ (float)i * 32.f, 0.f, 32.f, 32.f };
			atlas.x /= assetManager.texturesbg.width;
			atlas.width /= assetManager.texturesbg.width;
			atlas.y /= assetManager.texturesbg.height;
			atlas.height /= assetManager.texturesbg.height;

			ImGui::PushID(i);
			ImTextureID tex = (ImTextureID)(intptr_t)assetManager.texturesbg.id;

			if (ImGui::ImageButton(tex, { 35,35 }, { atlas.x, atlas.y }, { atlas.x + atlas.width, atlas.y + atlas.height })) {
				gameData.selectedBlockType = i;
			}

			ImGui::PopID();

			if (i % 10 != 0)
				ImGui::SameLine();
		}
		ImGui::End();

	}



	DrawFPS(10, 10);
	return true;
}

bool closeGame() {
	return true;
}
