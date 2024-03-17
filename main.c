#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 300
#define HEIGHT 600
#define PIPE_GAP 150
const float GRAVITY = 0.5;
const float JUMP_VELOCITY = -9;
float pipe_velocity = 1.5f;

typedef struct GameState {
  bool collided;
  bool pause;
  bool started;
  bool game_over;
} State;

State state = {
    .collided = false, .pause = false, .started = false, .game_over = false};

typedef struct Ball {
  Rectangle rect;
  // Vector2 pos_v;
  // Vector2 size_v;
  float velocity;
} ball;

void jump_ball(struct Ball *ball) {
  if (IsKeyPressed(KEY_SPACE)) {
    ball->velocity = JUMP_VELOCITY;
  }
}

void update_ball(struct Ball *ball) {
  ball->velocity += GRAVITY;
  ball->rect.y += ball->velocity;

  if (ball->rect.y >= HEIGHT - ball->rect.height) {
    ball->rect.y = HEIGHT - ball->rect.height;
  }
}

typedef struct Pipe {
  Vector2 top_pos_v;
  Vector2 bottom_pos_v;
  Vector2 bottom_size_v;
  Vector2 top_size_v;
} pipe;

void init_pipe(struct Pipe *pipe) {
  pipe->bottom_size_v = (Vector2){30.0f, 60.0f};
  pipe->top_size_v = (Vector2){30.0f, 60.0f};
}

void draw_pipe(struct Pipe *pipe) {
  DrawRectangleV(pipe->top_pos_v, pipe->top_size_v, GREEN);
  DrawRectangleV(pipe->bottom_pos_v, pipe->bottom_size_v, GREEN);
}

void update_pipe(struct Pipe *pipe) {
  time_t t;
  srand((unsigned)time(&t));

  pipe->top_pos_v.x -= pipe_velocity;
  pipe->bottom_pos_v.x -= pipe_velocity;

  if (pipe->top_pos_v.x <= -pipe->bottom_size_v.x) {
    pipe->top_pos_v = (Vector2){(float)GetScreenWidth(), 0};
    pipe->bottom_pos_v =
        (Vector2){(float)GetScreenWidth(), GetScreenHeight() - 30.0f};

    // random range formula
    // random_value = (rand() % (upper - lower + 1)) + lower
    srand(time(NULL));
    int a = (rand() % (500 - 150 + 1)) + 150;
    pipe->bottom_pos_v.y = a;
    pipe->bottom_size_v.y = GetScreenHeight();

    pipe->top_size_v.y = pipe->bottom_pos_v.y - PIPE_GAP;
  }
}

int main(void) {
  InitWindow(WIDTH, HEIGHT, "flappy bird");

  struct Ball ball;
  ball.rect = (Rectangle){70.0f, 100.0f, 30.0f, 30.0f};
  ball.velocity = 0;

  struct Pipe pipes[2] = {};

  for (int i = 0; i < sizeof(pipes) / sizeof(pipe); i++) {
    init_pipe(&pipes[i]);
  }

  pipes[0].top_pos_v = (Vector2){(float)GetScreenWidth(), 0};
  pipes[0].bottom_pos_v =
      (Vector2){(float)GetScreenWidth(), GetScreenHeight() - 60.0f};

  pipes[1].top_pos_v.x = pipes[0].top_pos_v.x - 150;
  pipes[1].bottom_pos_v.x = pipes[0].bottom_pos_v.x - 150;

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    jump_ball(&ball);
    update_ball(&ball);
    DrawRectangleRec(ball.rect, DARKBLUE);

    for (int i = 0; i < sizeof(pipes) / sizeof(pipe); i++) {
      update_pipe(&pipes[i]);
      draw_pipe(&pipes[i]);
    }

    EndDrawing();
  }
  CloseWindow();
  return 0;
}
