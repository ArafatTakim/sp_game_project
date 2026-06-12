#include <raylib.h>
#include <raymath.h>
#include <gameMain.h>
#include <block.h>
#include <iostream>
#include <rlImGui.h>
#include <imgui.h>


int main() {

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 850, "Sp Game");
	SetTargetFPS(240);

	rlImGuiSetup(true);

	if (!initGame()) return 0;


	while (!WindowShouldClose()) {

		BeginDrawing();
		ClearBackground(BLACK);

		rlImGuiBegin();

		if (!updateGame()) {
			return 0;
		}

		rlImGuiEnd();

		EndDrawing();
	}


	rlImGuiShutdown();

	CloseWindow();
	closeGame();

	std::cout << Block::BLOCKS_COUNT << "\n";

}