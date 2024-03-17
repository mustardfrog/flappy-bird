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
  float velocity;
  Sound jump_sound;
} ball;

void init_ball(struct Ball *ball) {
  ball->rect = (Rectangle){70.0f, 100.0f, 30.0f, 30.0f};
  ball->velocity = 0;
  ball->jump_sound = LoadSound("assets/sounds/8_bit_jumps/SFX_Jump_09.wav");
  SetSoundVolume(ball->jump_sound, 1.0);
}

void jump_ball(struct Ball *ball) {
  if (IsKeyPressed(KEY_SPACE)) {
    ball->velocity = JUMP_VELOCITY;
    PlaySound(ball->jump_sound);
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
  Rectangle top_rect;
  Rectangle bottom_rect;
} pipe;

void init_pipe(struct Pipe *pipe) {
  pipe->top_rect.width = 30.0f;
  pipe->top_rect.height = 30.0f;

  pipe->bottom_rect.width = 30.0f;
  pipe->bottom_rect.height = 30.0f;
}

void draw_pipe(struct Pipe *pipe) {
  DrawRectangleRec(pipe->top_rect, GREEN);
  DrawRectangleRec(pipe->bottom_rect, GREEN);
}

void update_pipe(struct Pipe *pipe) {
  time_t t;
  srand((unsigned)time(&t));

  pipe->top_rect.x -= pipe_velocity;
  pipe->bottom_rect.x -= pipe_velocity;

  if (pipe->top_rect.x <= -pipe->bottom_rect.width) {
    pipe->top_rect.x = (float)GetScreenWidth();
    pipe->top_rect.y = 0;
    pipe->bottom_rect.x = (float)GetScreenWidth();
    pipe->bottom_rect.y = GetScreenHeight() - 30.0f;
    // random range formula
    // random_value = (rand() % (upper - lower + 1)) + lower
    srand(time(NULL));
    int a = (rand() % (500 - 150 + 1)) + 150;
    pipe->bottom_rect.y = a;
    pipe->bottom_rect.height = GetScreenHeight();

    pipe->top_rect.height = pipe->bottom_rect.y - PIPE_GAP;
  }
}

void stop_pipe(pipe *p) {}

bool detect_collision(pipe *p, ball *b) {
  return CheckCollisionRecs(p->top_rect, b->rect) ||
         CheckCollisionRecs(p->bottom_rect, b->rect);
}

int main(void) {
  InitWindow(WIDTH, HEIGHT, "flappy bird");
  InitAudioDevice();

  struct Ball ball;
  init_ball(&ball);

  struct Pipe pipes[2] = {};

  for (int i = 0; i < sizeof(pipes) / sizeof(pipe); i++) {
    init_pipe(&pipes[i]);
  }

  pipes[0].top_rect.x = (float)GetScreenWidth();
  pipes[0].top_rect.y = 0;

  pipes[0].bottom_rect.x = (float)GetScreenWidth();
  pipes[0].bottom_rect.y = GetScreenHeight() - 60.0f;

  pipes[1].top_rect.x = pipes[0].top_rect.x - 150;
  pipes[1].bottom_rect.x = pipes[0].bottom_rect.x - 150;

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    if (!state.game_over) {

      ClearBackground(RAYWHITE);
      jump_ball(&ball);
      update_ball(&ball);
      DrawRectangleRec(ball.rect, DARKBLUE);

      for (int i = 0; i < sizeof(pipes) / sizeof(pipe); i++) {
        update_pipe(&pipes[i]);
        draw_pipe(&pipes[i]);
      }

      state.collided = detect_collision(&pipes[0], &ball) ||
                       detect_collision(&pipes[1], &ball);
      if (state.collided) {
        state.game_over = true;
      }
    }

    if (state.game_over) {
      DrawText("GAME OVER", GetScreenWidth() / 2, GetScreenHeight() / 2, 20,
               RED);
    }
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
