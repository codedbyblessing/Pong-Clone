/**
 * Author: [Your Name]
 * Assignment: Pong Clone
 * Date due: 2025-10-13, 11:59pm
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 **/

#include "CS3113/CS3113.h"
#include "raylib.h"
#include <vector>
#include <cmath>


constexpr int SCREEN_WIDTH  = 800;
constexpr int SCREEN_HEIGHT = 450;
constexpr int FPS = 120;

AppStatus gAppStatus = RUNNING;
float gFrameCount = 0.0f;

struct Paddle {
    Vector2 pos;
    Texture2D texture;
    float speed = 400.0f;
};

struct Ball {
    Vector2 pos;
    Vector2 vel;
    Texture2D texture;
    bool active;
};

Paddle leftPaddle, rightPaddle;
std::vector<Ball> balls;
Texture2D bg;

int leftScore = 0, rightScore = 0;
int numBalls = 1;

void initialise();
void begingame();
void update();
void render();
void shutdown();
bool singleplayer;
bool checkCollision(Rectangle a, Rectangle b);
bool gameOver = false;

bool checkCollision(Rectangle a, Rectangle b) {
    float xdist = fabs((a.x + a.width / 2) - (b.x + b.width / 2))
                - ((a.width + b.width) / 2);
    float ydist = fabs((a.y + a.height / 2) - (b.y + b.height / 2))
                - ((a.height + b.height) / 2);
    return (xdist < 0 && ydist < 0);
}


void initialise() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong Clone (CS3113)");
    SetTargetFPS(FPS);

    bg = LoadTexture("assets/forest.png");
    leftPaddle.texture  = LoadTexture("assets/RG.png");
    rightPaddle.texture = LoadTexture("assets/YG.png");

    leftPaddle.pos = { 60.0f, (SCREEN_HEIGHT - leftPaddle.texture.height) / 2.0f };
    rightPaddle.pos = { (float)(SCREEN_WIDTH - 60 - rightPaddle.texture.width),
                        (SCREEN_HEIGHT - rightPaddle.texture.height) / 2.0f };

    // 3 balls
    Texture2D ballTex = LoadTexture("assets/pumpkin.png");
    balls.resize(3);
    for (int i = 0; i < 3; i++) {
        balls[i].texture = ballTex;
        balls[i].active = (i == 0);
        //initialize speed
        balls[i].vel = { 0.0f, 0.0f };
        //render from center
        balls[i].pos = {
            (SCREEN_WIDTH - ballTex.width) / 2.0f,
            (SCREEN_HEIGHT - ballTex.height) / 2.0f
        };
    }
    numBalls = 1; //automatically
}


void begingame() {
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) {
        gAppStatus = TERMINATED;
    }
    if (IsKeyPressed(KEY_ONE)) numBalls = 1;
    if (IsKeyPressed(KEY_TWO)) numBalls = 2;
    if (IsKeyPressed(KEY_THREE)) numBalls = 3;

    for (int i = 0; i < 3; i++) {
        balls[i].active = (i < numBalls);
        if (!balls[i].active) {
            balls[i].vel = { 0.0f, 0.0f };
            balls[i].pos = {
                (SCREEN_WIDTH - balls[i].texture.width) / 2.0f,
                (SCREEN_HEIGHT - balls[i].texture.height) / 2.0f
            };
        }
    }
}

void update() {
    if (gameOver) {
        if (IsKeyPressed(KEY_ENTER)) Reset();
        return;
    }
    float seconds = (float)GetTime();
    float deltaTime = seconds - gSpeedCount;
    gFrameCount = seconds;

    // Paddle movement
    if (IsKeyDown(KEY_W)) leftPaddle.pos.y -= leftPaddle.speed * deltaTime;
    if (IsKeyDown(KEY_S)) leftPaddle.pos.y += leftPaddle.speed * deltaTime;

    if (IsKeyDown(KEY_UP)) rightPaddle.pos.y -= rightPaddle.speed * deltaTime;
    if (IsKeyDown(KEY_DOWN)) rightPaddle.pos.y += rightPaddle.speed * deltaTime;

    //paddles
    if (leftPaddle.pos.y < 0) leftPaddle.pos.y = 0;
    if (leftPaddle.pos.y + leftPaddle.texture.height > SCREEN_HEIGHT)
        leftPaddle.pos.y = SCREEN_HEIGHT - leftPaddle.texture.height;

    if (rightPaddle.pos.y < 0) rightPaddle.pos.y = 0;
    if (rightPaddle.pos.y + rightPaddle.texture.height > SCREEN_HEIGHT)
        rightPaddle.pos.y = SCREEN_HEIGHT - rightPaddle.texture.height;
    

    // Ball logic
    for (auto &b : balls) {
        if (!b.active) continue;

        // Move only if velocity != 0
        if (b.vel.x != 0.0f || b.vel.y != 0.0f) {
            b.pos.x += b.vel.x * deltaTime;
            b.pos.y += b.vel.y * deltaTime;
        }

        // Bouncing off top/bottom
        if (b.pos.y <= 0) {
            b.pos.y = 0;
            b.vel.y *= -1;
        }
        if (b.pos.y + b.texture.height >= SCREEN_HEIGHT) {
            b.pos.y = SCREEN_HEIGHT - b.texture.height;
            b.vel.y *= -1;
        }

        // Paddle collision
        Rectangle ballRect  = { b.pos.x, b.pos.y, (float)b.texture.width, (float)b.texture.height };
        Rectangle leftRect  = { leftPaddle.pos.x, leftPaddle.pos.y,
                                (float)leftPaddle.texture.width, (float)leftPaddle.texture.height };
        Rectangle rightRect = { rightPaddle.pos.x, rightPaddle.pos.y,
                                (float)rightPaddle.texture.width, (float)rightPaddle.texture.height };

        if (checkCollision(ballRect, leftRect)) {
            // kick off to the right
            b.vel.x = fabs(b.vel.x > 0 ? b.vel.x : 200.0f);
            b.vel.y = ((b.pos.y + b.texture.height/2.0f) - (leftPaddle.pos.y + leftPaddle.texture.height / 2.0f)) * 2.0f;
        }
        if (checkCollision(ballRect, rightRect)) {
            // kick off to the left
            b.vel.x = -fabs(b.vel.x < 0 ? b.vel.x : 200.0f);
            b.vel.y = ((b.pos.y + b.texture.height/2.0f) - (rightPaddle.pos.y + rightPaddle.texture.height / 2.0f)) * 2.0f;
        }

        // Change number of pumpkins
        if (IsKeyPressed(KEY_ONE)) numBalls = 1;
        if (IsKeyPressed(KEY_TWO)) numBalls = 2;
        if (IsKeyPressed(KEY_THREE)) numBalls = 3;

        for (int i = 0; i < (int)balls.size(); i++)
            balls[i].active = (i < numBalls);

        if (gameOver) {
            if (IsKeyPressed(KEY_ENTER)) Restart();
            return;
        }

        // Out of bounds
        // Scoring
        if (b.pos.x < 0) { rightScore++; winner = 2; gameOver = true; }
        else if (b.pos.x > SCREEN_WIDTH) { leftScore++; winner = 1; gameOver = true; }
    }
}

void render() {
    BeginDrawing();
    ClearBackground(BLACK);

    // Draw background at original size
    DrawTexture(bg, 0, 0, WHITE);

    // Draw paddles
    DrawTexture(leftPaddle.texture, leftPaddle.pos.x, leftPaddle.pos.y, WHITE);
    DrawTextureEx(rightPaddle.texture,
                  { rightPaddle.pos.x + (float)rightPaddle.texture.width, rightPaddle.pos.y },
                  0.0f, -1.0f, WHITE);

    // Draw balls
    for (auto &b : balls) {
        if (!b.active) continue;
        DrawTexture(b.texture, b.pos.x, b.pos.y, WHITE);
    }

    // Divider
    DrawRectangle(SCREEN_WIDTH / 2 - 2, 0, 4, SCREEN_HEIGHT, WHITE);

    // Scores
    DrawText(TextFormat("%d", leftScore), SCREEN_WIDTH / 4, 20, 40, WHITE);
    DrawText(TextFormat("%d", rightScore), SCREEN_WIDTH * 3 / 4, 20, 40, WHITE);

    if (gameOver) {
        const char* msg = (winner == 1 ? "Foxy Wins!" : "Chica Wins!");
        int fontSize = 40;
        int textWidth = MeasureText(msg, fontSize);
        DrawText(msg, SCREEN_WIDTH / 2 - textWidth / 2, SCREEN_HEIGHT / 2 - fontSize / 2, fontSize, RED);

        const char* subMsg = "Press [ENTER] to Restart\nPress [1], [2], or [3] to Change Pumpkin Count \n";
        int subFontSize = 20;
        int subTextWidth = MeasureText(subMsg, subFontSize);
        DrawText(subMsg, SCREEN_WIDTH / 2 - subTextWidth / 2, SCREEN_HEIGHT / 2 + 40, subFontSize, WHITE);
    }

        const char* modeMsg = singlePlayer ? "Single Player [T]: ON" : "Single Player [T]: OFF";
        int fontSize = 20;
        int textWidth = MeasureText(modeMsg, fontSize);
        DrawText(modeMsg, SCREEN_WIDTH / 2 - textWidth / 2, 20, fontSize, WHITE);


    EndDrawing();
}


void shutdown() {
    UnloadTexture(bg);
    UnloadTexture(leftPaddle.texture);
    UnloadTexture(rightPaddle.texture);
    CloseWindow();
}


int main() {
    initialise();
    while (gAppStatus == RUNNING) {
        begingame();
        update();
        render();
    }
    shutdown();
    return 0;
}
