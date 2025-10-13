/**
* Author: [Blessing Yeboah
* Assignment: Pong Clone
* Date due: 2025-10-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <cmath>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

// Game status
enum AppStatus { RUNNING, EXIT };
AppStatus gAppStatus = RUNNING;

struct Ball {
    Vector2 pos;
    Vector2 vel;
    Texture2D tex;
    bool active = false;
    int lastHit = 0;
};

//checking for overlap with the paddle rectangles
static bool RectOverlap(Rectangle r1, Rectangle r2) {
    float dx = fabs((r1.x + r1.width / 2) - (r2.x + r2.width / 2)) - ((r1.width + r2.width) / 2);
    float dy = fabs((r1.y + r1.height / 2) - (r2.y + r2.height / 2)) - ((r1.height + r2.height) / 2);
    return dx < 0 && dy < 0;
}

class PongClone {
public:
    Texture2D bg, leftTex, rightTex, ballTex;
    Vector2 leftPaddle, rightPaddle;
    vector<Ball> balls;

    int numBalls = 1;
    int leftScore = 0, rightScore = 0;
    bool singlePlayer = false;
    bool gameOver = false;
    int winner = 0;

    void Init() {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong with Foxy and Chica");
        SetTargetFPS(60);

        bg = LoadTexture("assets/forest.png");
        leftTex = LoadTexture("assets/RG.png");
        rightTex = LoadTexture("assets/YG.png");
        ballTex = LoadTexture("assets/pumpkin.png");

        leftPaddle = { 80.0f, SCREEN_HEIGHT / 2.0f - leftTex.height / 2.0f };
        rightPaddle = { static_cast<float>(SCREEN_WIDTH - 80 - rightTex.width),
                        SCREEN_HEIGHT / 2.0f - rightTex.height / 2.0f };

        balls.resize(3);
        for (int i = 0; i < 3; i++) {
            balls[i].pos = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
            balls[i].vel = { (i % 2 == 0 ? 250.0f : -250.0f), (i + 1) * 50.0f };
            balls[i].tex = ballTex;
            balls[i].active = (i == 0);
        }
    }

    void Reset() {
        winner = 0;
        gameOver = false;
        for (auto &b : balls) {
            b.pos = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
            b.lastHit = 0;
        }
    }

    void HandleInput() {
        if (IsKeyPressed(KEY_T)) singlePlayer = !singlePlayer;
        if (IsKeyPressed(KEY_ONE)) numBalls = 1;
        if (IsKeyPressed(KEY_TWO)) numBalls = 2;
        if (IsKeyPressed(KEY_THREE)) numBalls = 3;

        for (int i = 0; i < (int)balls.size(); i++)
            balls[i].active = (i < numBalls);

        // Exit game logic
        if (IsKeyPressed(KEY_Q) || WindowShouldClose())
            gAppStatus = EXIT;
    }

    void Update(float dt) {
        if (gameOver) {
            if (IsKeyPressed(KEY_ENTER)) Reset();
            return;
        }

        // Left paddle (player 1)
        if (IsKeyDown(KEY_W)) leftPaddle.y -= 400 * dt;
        if (IsKeyDown(KEY_S)) leftPaddle.y += 400 * dt;
        leftPaddle.y = Clamp(leftPaddle.y, 0.0f, (float)SCREEN_HEIGHT - leftTex.height);

        // Right paddle
        if (singlePlayer) {
            static bool movingDown = true;
            float speed = 200.0f;
            if (movingDown) {
                rightPaddle.y += speed * dt;
                if (rightPaddle.y + rightTex.height >= SCREEN_HEIGHT) movingDown = false;
            } else {
                rightPaddle.y -= speed * dt;
                if (rightPaddle.y <= 0) movingDown = true;
            }
        } else {
            if (IsKeyDown(KEY_UP)) rightPaddle.y -= 400 * dt;
            if (IsKeyDown(KEY_DOWN)) rightPaddle.y += 400 * dt;
        }
        rightPaddle.y = Clamp(rightPaddle.y, 0.0f, (float)SCREEN_HEIGHT - rightTex.height);

        // Pumpkins (ball used for pong)
        for (auto &b : balls) {
            if (!b.active) continue;

            b.pos.x += b.vel.x * dt;
            b.pos.y += b.vel.y * dt;

            Rectangle ballBox = { b.pos.x, b.pos.y, (float)b.tex.width, (float)b.tex.height };
            Rectangle leftBox = { leftPaddle.x, leftPaddle.y, (float)leftTex.width, (float)leftTex.height };
            Rectangle rightBox = { rightPaddle.x, rightPaddle.y, (float)rightTex.width, (float)rightTex.height };

            if (b.pos.y <= 0 || b.pos.y + b.tex.height >= SCREEN_HEIGHT)
                b.vel.y *= -1;

            const float overlap = -45.0f;
            Rectangle leftHit = { leftBox.x - overlap, leftBox.y - overlap,
                                  leftBox.width + overlap * 2, leftBox.height + overlap * 2 };
            Rectangle rightHit = { rightBox.x - overlap, rightBox.y - overlap,
                                   rightBox.width + overlap * 2, rightBox.height + overlap * 2 };

            if (RectOverlap(ballBox, leftHit)) { b.vel.x = fabs(b.vel.x); b.lastHit = 1; }
            if (RectOverlap(ballBox, rightHit)) { b.vel.x = -fabs(b.vel.x); b.lastHit = 2; }

            // Scoring
            if (b.pos.x < 0) { rightScore++; winner = 2; gameOver = true; }
            else if (b.pos.x > SCREEN_WIDTH) { leftScore++; winner = 1; gameOver = true; }
        }
    }

    void Render() {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(bg, 0, 0, WHITE);
        DrawTextureEx(leftTex, leftPaddle, 0, 1.0f, WHITE);
        DrawTextureEx(rightTex, rightPaddle, 0, 1.0f, WHITE);

        for (auto &b : balls)
            if (b.active)
                DrawTexture(b.tex, b.pos.x, b.pos.y, WHITE);

        DrawRectangle(SCREEN_WIDTH / 2 - 2, 0, 4, SCREEN_HEIGHT, WHITE);
        DrawText(TextFormat("%d", leftScore), SCREEN_WIDTH / 4, 20, 40, WHITE);
        DrawText(TextFormat("%d", rightScore), SCREEN_WIDTH * 3 / 4, 20, 40, WHITE);

        if (gameOver) {
            const char* msg = (winner == 1 ? "Foxy Wins!" : "Chica Wins!");
            int fontSize = 40;
            int textWidth = MeasureText(msg, fontSize);
            DrawText(msg, SCREEN_WIDTH / 2 - textWidth / 2, SCREEN_HEIGHT / 2 - fontSize / 2, fontSize, RED);

            const char* subMsg = "Press [ENTER] to Restart\nPress [1], [2], or [3] to Change Pumpkin Count in game!";
            int subFontSize = 20;
            int subTextWidth = MeasureText(subMsg, subFontSize);
            DrawText(subMsg, SCREEN_WIDTH / 2 - subTextWidth / 2, SCREEN_HEIGHT / 2 + 40, subFontSize, WHITE);
        }

        DrawText(singlePlayer ? "Single Player [T]: ON" : "Single Player [T]: OFF",
                 20, SCREEN_HEIGHT - 40, 20, WHITE);
        EndDrawing();
    }

    void Shutdown() {
        UnloadTexture(bg);
        UnloadTexture(leftTex);
        UnloadTexture(rightTex);
        UnloadTexture(ballTex);
        CloseWindow();
    }
};

int main(void)
{
    PongClone game;
    game.Init();

    while (gAppStatus == RUNNING)
    {
        game.HandleInput();
        float dt = GetFrameTime();
        game.Update(dt);
        game.Render();
    }

    game.Shutdown();
    return 0;
}
