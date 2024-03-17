#include "raylib.h"
int main(void) {
  const int screenWidth = 400;
  const int screenHeight = 600;

  InitWindow(screenWidth, screenHeight, "flappy bird");

  Vector2 ballPosition = {100.0f, 100.0f};
  Color ballColor = DARKBLUE;

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second

  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawCircleV(ballPosition, 20, ballColor);

    EndDrawing();
  }

  CloseWindow(); // Close window and OpenGL context

  return 0;
}
