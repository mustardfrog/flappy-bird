#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 300
#define HEIGHT 600
#define PIPE_GAP 150
#define ROW 5
#define COL 5

const float GRAVITY = 0.5;
const float JUMP_VELOCITY = -9;
float pipe_velocity = 1.5f;

typedef struct {
  bool collided;
  bool pause;
  bool started;
  bool game_over;
  int score;
  int best_score;
} State;

typedef struct Ball {
  Rectangle rect;
  float velocity;
  Sound jump_sound;
  Sound boom_sound;
  Texture2D explosion_texture;
} ball;

void init_ball(struct Ball *ball) {
  ball->rect = (Rectangle){70.0f, 100.0f, 30.0f, 30.0f};
  ball->velocity = 0;
  ball->jump_sound = LoadSound("assets/sounds/8_bit_jumps/SFX_Jump_09.wav");
  ball->boom_sound = LoadSound("assets/sounds/boom.wav");
  ball->explosion_texture = LoadTexture("assets/explosion.png");
  SetSoundVolume(ball->jump_sound, 1.0);
  SetSoundVolume(ball->boom_sound, .8);
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

void clean_ball(ball *b) {
  UnloadSound(b->jump_sound);
  UnloadSound(b->boom_sound);
  UnloadTexture(b->explosion_texture);
}

void play_boom_animation(ball *b) {}

typedef struct Pipe {
  Rectangle top_rect;
  Rectangle bottom_rect;
} Pipe;

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
  // time_t t;
  // srand((unsigned)time(&t));

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

bool detect_collision(Pipe *p, ball *b) {
  return CheckCollisionRecs(p->top_rect, b->rect) ||
         CheckCollisionRecs(p->bottom_rect, b->rect);
}

int main(void) {
  InitWindow(WIDTH, HEIGHT, "Flappy Bird");
  InitAudioDevice();

  State state = {
      .collided = false,
      .pause = false,
      .started = false,
      .game_over = false,
      .score = 0,
      .best_score = 0,
  };

  struct Ball ball;
  init_ball(&ball);

  float frame_width = (float)ball.explosion_texture.width / ROW;
  float frame_height = (float)ball.explosion_texture.height / COL;
  int current_frame = 0;
  int current_line = 0;

  bool active = false;

  int frame_counter = 0;

  Rectangle frame_rect = (Rectangle){0, 0, frame_width, frame_height};

  struct Pipe pipes[2] = {};
  for (int i = 0; i < sizeof(pipes) / sizeof(Pipe); i++) {
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

      // dirtiest and stupidest solution
      if (ball.rect.x >= pipes[0].top_rect.x &&
              ball.rect.x <= pipes[0].top_rect.x + 1 ||
          ball.rect.x >= pipes[1].top_rect.x &&
              ball.rect.x <= pipes[1].top_rect.x + 1) {
        state.score++;
      }

      jump_ball(&ball);
      ClearBackground(RAYWHITE);
      update_ball(&ball);
      DrawRectangleRec(ball.rect, DARKBLUE);

      for (int i = 0; i < sizeof(pipes) / sizeof(Pipe); i++) {
        update_pipe(&pipes[i]);
        draw_pipe(&pipes[i]);
      }

      state.collided = detect_collision(&pipes[0], &ball) ||
                       detect_collision(&pipes[1], &ball);

      DrawText(TextFormat("score: %d", state.score), 10, 30, 20, RED);

      if (state.collided) {
        PlaySound(ball.boom_sound);
        active = true;
        state.game_over = true;
      }
    }

    if (state.game_over) {
      DrawText("GAME OVER", GetScreenWidth() / 2, GetScreenHeight() / 2, 20,
               RED);
      if (active) {
        frame_counter++;
        if (frame_counter > 2) {
          current_frame++;

          if (current_frame >= ROW) {
            current_frame = 0;
            current_line++;

            if (current_line >= COL) {
              current_line = 0;
              active = false;
            }
          }
          frame_counter = 0;
        }
      }

      frame_rect.x = frame_width * current_frame;
      frame_rect.y = frame_height * current_line;
      if (active) {
        float start_time = GetTime();
        DrawTextureRec(ball.explosion_texture, frame_rect,
                       (Vector2){ball.rect.x - frame_width / 2,
                                 ball.rect.y - frame_height / 2},
                       WHITE);
        if (current_frame == ROW && current_line == COL) {
          ClearBackground(BLACK);
        }
      }
    }
    EndDrawing();
  }
  clean_ball(&ball);
  CloseAudioDevice();
  CloseWindow();
  return 0;
}
