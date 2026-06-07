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

// ================= PLAYER =================
struct Player {
    Vector2 position;
    float speed;
    Texture2D texture;
};

// ================= GAME DATA =================
struct GameData {
    GameMap gameMap;
    Camera2D camera;

    int creativeSelectedBlock = Block::dirt;

    Vector2 selectionStart = {0,0};
    Vector2 selectionEnd = {0,0};

    Structure copyStructure;
    char saveName[100] = {};

    Player player;
} gameData;

// ================= GLOBAL =================
AssetManager assetManager;
bool showImgui = false;

// ================= INIT =================
bool initGame() {
    assetManager.loadAll();
    generateWorld(gameData.gameMap);

    gameData.camera.target = { 300, 100 };
    gameData.camera.offset = { GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    gameData.camera.rotation = 0.0f;
    gameData.camera.zoom = 20.0f;   // Zoom out to see more world

    gameData.player.position = { 300, 100 };
    gameData.player.speed = 0.5f;
    gameData.player.texture = assetManager.player;

    return true;
}

// ================= UPDATE =================
bool updateGame() {

    float deltaTime = GetFrameTime();
    if (deltaTime > 1.f / 5) deltaTime = 1 / 5.f;

    gameData.camera.offset = { GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };

    ClearBackground({ 75,75,150,255 });

    // ================= INPUT TOGGLE =================
    if (IsKeyPressed(KEY_C)) {
        showImgui = !showImgui;
    }

    // ================= PLAYER MOVEMENT =================
    Vector2 dir = {0, 0};

    if (IsKeyDown(KEY_LEFT)) dir.x -= 1;
    if (IsKeyDown(KEY_RIGHT)) dir.x += 1;
    if (IsKeyDown(KEY_UP)) dir.y -= 1;
    if (IsKeyDown(KEY_DOWN)) dir.y += 1;

    gameData.player.position.x += dir.x * gameData.player.speed;
    gameData.player.position.y += dir.y * gameData.player.speed;

    // Clamp player position to map bounds
    gameData.player.position.x = Clamp(gameData.player.position.x, 0.0f, (float)gameData.gameMap.w);
    gameData.player.position.y = Clamp(gameData.player.position.y, 0.0f, (float)gameData.gameMap.h);

    // ================= CAMERA FOLLOW =================
    gameData.camera.target = gameData.player.position;

    // ================= MOUSE WORLD POSITION =================
    Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);
    int blockX = (int)floor(worldPos.x);
    int blockY = (int)floor(worldPos.y);

    // ================= EDIT MODE =================
    if (showImgui) {

        if (IsKeyPressed(KEY_ONE)) {
            gameData.selectionStart = { (float)blockX, (float)blockY };
        }

        if (IsKeyPressed(KEY_TWO)) {
            gameData.selectionEnd = { (float)blockX, (float)blockY };
        }

        if (IsKeyPressed(KEY_THREE)) {
            gameData.copyStructure.pasteIntoMap(
                gameData.gameMap,
                { (float)blockX, (float)blockY }
            );
        }

        if (gameData.selectionStart.x > gameData.selectionEnd.x)
            std::swap(gameData.selectionStart.x, gameData.selectionEnd.x);

        if (gameData.selectionStart.y > gameData.selectionEnd.y)
            std::swap(gameData.selectionStart.y, gameData.selectionEnd.y);
    }

    // ================= WORLD EDIT =================
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

    // ================= RENDER =================
    BeginMode2D(gameData.camera);

    Vector2 topLeftView = GetScreenToWorld2D({0,0}, gameData.camera);
    Vector2 bottomRightView = GetScreenToWorld2D(
        {(float)GetScreenWidth(), (float)GetScreenHeight()},
        gameData.camera
    );

    int startX = Clamp((int)floor(topLeftView.x) - 1, 0, gameData.gameMap.w);
    int endX   = Clamp((int)ceil(bottomRightView.x) - 1, 0, gameData.gameMap.w);
    int startY = Clamp((int)floor(topLeftView.y) - 1, 0, gameData.gameMap.h);
    int endY   = Clamp((int)ceil(bottomRightView.y) - 1, 0, gameData.gameMap.h);

    // WORLD DRAW
    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {

            auto& b = gameData.gameMap.getBlock(x, y);
            if (b.type == Block::air) continue;

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

    // PLAYER DRAW (FIXED)
    DrawTextureEx(
        gameData.player.texture,
        gameData.player.position,
        0.0f,
        0.1f,
        WHITE
    );

    // CURSOR FRAME
    DrawTexturePro(
        assetManager.frame,
        {0,0,(float)assetManager.frame.width,(float)assetManager.frame.height},
        { (float)blockX, (float)blockY, 1, 1 },
        { 0,0 },
        0.0f,
        WHITE
    );

    // SELECTION BOX
    if (showImgui) {
        Rectangle rect;
        rect.x = gameData.selectionStart.x;
        rect.y = gameData.selectionStart.y;
        rect.width = gameData.selectionEnd.x - gameData.selectionStart.x + 1;
        rect.height = gameData.selectionEnd.y - gameData.selectionStart.y + 1;

        DrawRectangleLinesEx(rect, 0.1f, { 20,101,250,255 });
    }

    EndMode2D();

    // ================= UI =================
    if (showImgui) {

        ImGui::Begin("Game control");

        ImGui::SliderFloat("Camera zoom:", &gameData.camera.zoom, 0.1f, 10);
        ImGui::SliderFloat("Player speed:", &gameData.player.speed, 1, 10);

        ImGui::InputText("File name", gameData.saveName, sizeof(gameData.saveName));

        ImGui::End();
    }

    DrawFPS(10, 10);
    return true;
}

// ================= CLOSE =================
void closeGame() {
    UnloadTexture(gameData.player.texture);
}
