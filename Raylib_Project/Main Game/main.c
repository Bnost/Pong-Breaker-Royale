#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define BRICK_ROWS 8
#define BRICK_COLS 14
#define MAX_SCORES 5

typedef enum {
    STATE_MENU,
    STATE_CHOICE,
    STATE_HIGHSCORE,
    STATE_GAME,
} GameState;

typedef struct {
    Rectangle rect;
    float speed;
    Color color;
} Paddle;

typedef struct {
    Rectangle rect;
    bool active;  // true ise ekranda duruyor, false ise kırılmış
    Color color;
} Brick;

typedef struct {
    Vector2 position;
    Vector2 speed;
    float radius;
    Color color;
} Ball;

typedef struct {
    char name[16];
    int score;
} ScoreEntry;

ScoreEntry topScores[MAX_SCORES];
void LoadScores();
void SaveScore(const char* name, int newScore);
void DrawLeaderboard();

int score = 0;
bool isBotMode = false;

int main(void)
{
    int screenWidth = 800;
    int screenHeight = 450;
    GameState currentScreen = STATE_MENU;
    int menuSelection = 1;
    int choiceSelection = 1; // 1: 2 Oyuncu, 2: Bot vs

    float baseScreenWidth = 800.0f;
    float baseScreenHeight = 450.0f;
    float baseBallRadius = 10.0f;
    float baseBallSpeed = 300.0f;
    float basePaddleWidth = 80.0f;
    float basePaddleHeight = 20.0f;
    float basePaddleSpeed = 500.0f;

    float scaleX = screenWidth / baseScreenWidth;
    float scaleY = screenHeight / baseScreenHeight;

    Ball ball = { {screenWidth / 2.0f, screenHeight / 2.0f + 135 * scaleY}, {baseBallSpeed * scaleX, -baseBallSpeed * scaleY}, baseBallRadius * ((scaleX + scaleY) / 2.0f), SKYBLUE };
    Paddle player1 = { {screenWidth / 2.0f - (basePaddleWidth/2.0f) * scaleX, screenHeight / 2.0f + 150 * scaleY, basePaddleWidth * scaleX, basePaddleHeight * scaleY}, basePaddleSpeed * scaleX, GREEN };
    Paddle player2 = { {screenWidth / 2.0f - (basePaddleWidth/2.0f) * scaleX, screenHeight / 2.0f - 170 * scaleY, basePaddleWidth * scaleX, basePaddleHeight * scaleY}, basePaddleSpeed * scaleX, GREEN };
    
    Brick bricks[BRICK_ROWS][BRICK_COLS];
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            bricks[i][j].rect = (Rectangle){
                j * (baseScreenWidth / BRICK_COLS) * scaleX + (baseScreenWidth / BRICK_COLS * 0.05f) * scaleX,
                (baseScreenHeight * 0.35f + i * (baseScreenHeight / 25.0f)) * scaleY,
                (baseScreenWidth / BRICK_COLS * 0.9f) * scaleX,
                (baseScreenHeight / 30.0f) * scaleY
            };
            bricks[i][j].active = true;
            bricks[i][j].color = WHITE;
        }
    }
    Rectangle backbutton = { 10, 10, 80, 30 };

    InitWindow(screenWidth, screenHeight, "Raylib - Pong Breaker Royale");
    InitAudioDevice(); 
    Sound bam = LoadSound("Pop.ogg"); 

    LoadScores();

    SetTargetFPS(60);
    srand(time(NULL));

    while (!WindowShouldClose())
    {
        SetExitKey(KEY_NULL);

        // F11 ile tam ekran geçişi
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        // Her karede ekran boyutunu güncelle
        int prevScreenWidth = screenWidth;
        int prevScreenHeight = screenHeight;
        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();
        scaleX = screenWidth / baseScreenWidth;
        scaleY = screenHeight / baseScreenHeight;

        // Eğer ekran boyutu değiştiyse oyun durumunda nesneleri yeniden ölçekle
        static int lastScreenWidth = 800, lastScreenHeight = 450;
        if ((screenWidth != lastScreenWidth || screenHeight != lastScreenHeight) && currentScreen == STATE_GAME) {
            player1.rect.width = basePaddleWidth * scaleX;
            player1.rect.height = basePaddleHeight * scaleY;
            player1.rect.x = (player1.rect.x / prevScreenWidth) * screenWidth;
            player1.rect.y = (player1.rect.y / prevScreenHeight) * screenHeight;
            player1.speed = basePaddleSpeed * scaleX;

            player2.rect.width = basePaddleWidth * scaleX;
            player2.rect.height = basePaddleHeight * scaleY;
            player2.rect.x = (player2.rect.x / prevScreenWidth) * screenWidth;
            player2.rect.y = (player2.rect.y / prevScreenHeight) * screenHeight;
            player2.speed = basePaddleSpeed * scaleX;

            // Topun pozisyonunu ve hızını orantıla
            ball.position.x = (ball.position.x / prevScreenWidth) * screenWidth;
            ball.position.y = (ball.position.y / prevScreenHeight) * screenHeight;
            ball.radius = baseBallRadius * ((scaleX + scaleY) / 2.0f);
            ball.speed.x = (ball.speed.x > 0 ? 1 : -1) * baseBallSpeed * scaleX;
            ball.speed.y = (ball.speed.y > 0 ? 1 : -1) * baseBallSpeed * scaleY;

            // Tuğlaları yeniden ölçekle
            for (int i = 0; i < BRICK_ROWS; i++) {
                for (int j = 0; j < BRICK_COLS; j++) {
                    bricks[i][j].rect.x = j * (baseScreenWidth / BRICK_COLS) * scaleX + (baseScreenWidth / BRICK_COLS * 0.05f) * scaleX;
                    bricks[i][j].rect.y = (baseScreenHeight * 0.35f + i * (baseScreenHeight / 25.0f)) * scaleY;
                    bricks[i][j].rect.width = (baseScreenWidth / BRICK_COLS * 0.9f) * scaleX;
                    bricks[i][j].rect.height = (baseScreenHeight / 30.0f) * scaleY;
                }
            }

            lastScreenWidth = screenWidth;
            lastScreenHeight = screenHeight;
        }

        BeginDrawing();

        switch (currentScreen)
        {
        case STATE_MENU:
            ClearBackground(RAYWHITE);
            DrawText("PONG BREAKER ROYALE OYNA", screenWidth * 0.25f, screenHeight * 0.44f, screenHeight * 0.044f, BLACK);

            Rectangle playRect = { screenWidth / 2 - screenWidth * 0.062f, screenHeight * 0.56f, screenWidth * 0.15f, screenHeight * 0.055f };
            Rectangle scoreRect = { screenWidth / 2 - screenWidth * 0.062f, screenHeight * 0.67f, screenWidth * 0.15f, screenHeight * 0.055f };
            Rectangle exitRect = { screenWidth / 2 - screenWidth * 0.062f, screenHeight * 0.78f, screenWidth * 0.15f, screenHeight * 0.055f };

            Vector2 mousePos = GetMousePosition();
            int hovered = 0;
            if (CheckCollisionPointRec(mousePos, playRect)) hovered = 1;
            else if (CheckCollisionPointRec(mousePos, scoreRect)) hovered = 2;
            else if (CheckCollisionPointRec(mousePos, exitRect)) hovered = 3;

            IsKeyPressed(KEY_DOWN) && menuSelection < 3 ? menuSelection++ : 0;
            IsKeyPressed(KEY_UP) && menuSelection > 1 ? menuSelection-- : 0;

            if (hovered) menuSelection = hovered;

            DrawText("Oyna", playRect.x, playRect.y, screenHeight * 0.044f, (menuSelection == 1) ? RED : BLACK);
            DrawText("Skorlar", scoreRect.x, scoreRect.y, screenHeight * 0.044f, (menuSelection == 2) ? RED : BLACK);
            DrawText("Cikis", exitRect.x, exitRect.y, screenHeight * 0.044f, (menuSelection == 3) ? RED : BLACK);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (hovered == 1) currentScreen = STATE_CHOICE;
                else if (hovered == 2) currentScreen = STATE_HIGHSCORE;
                else if (hovered == 3) CloseWindow();
            }
            if (IsKeyPressed(KEY_ENTER)) {
                if (menuSelection == 1) currentScreen = STATE_CHOICE;
                else if (menuSelection == 2) currentScreen = STATE_HIGHSCORE;
                else if (menuSelection == 3) CloseWindow();
            }
            break;

        case STATE_CHOICE:
            ClearBackground(RAYWHITE);
            DrawText("OYUN MODU SECIN", screenWidth * 0.35f, screenHeight * 0.30f, screenHeight * 0.050f, BLACK);

            Rectangle p2Rect = { screenWidth * 0.25f, screenHeight * 0.50f, screenWidth * 0.20f, screenHeight * 0.06f };
            Rectangle botRect = { screenWidth * 0.55f, screenHeight * 0.50f, screenWidth * 0.20f, screenHeight * 0.06f };

            Vector2 choiceMousePos = GetMousePosition();
            int choiceHovered = 0;
            if (CheckCollisionPointRec(choiceMousePos, p2Rect)) choiceHovered = 1;
            else if (CheckCollisionPointRec(choiceMousePos, botRect)) choiceHovered = 2;

            IsKeyPressed(KEY_RIGHT) && choiceSelection < 2 ? choiceSelection++ : 0;
            IsKeyPressed(KEY_LEFT) && choiceSelection > 1 ? choiceSelection-- : 0;

            if (choiceHovered) choiceSelection = choiceHovered;

            DrawText("2 Oyuncu", p2Rect.x, p2Rect.y, screenHeight * 0.050f, (choiceSelection == 1) ? RED : BLACK);
            DrawText("Bot vs", botRect.x, botRect.y, screenHeight * 0.050f, (choiceSelection == 2) ? RED : BLACK);

            bool startGame = false;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (choiceHovered == 1 || choiceHovered == 2) {
                    startGame = true;
                }
            }
            if (IsKeyPressed(KEY_ENTER)) {
                startGame = true;
            }

            if (startGame) {
                isBotMode = (choiceSelection == 2);
                
                // Oyunu Başlatmadan Önce Nesneleri Sıfırla
                score = 0;
                player1.rect = (Rectangle){ screenWidth / 2 - (basePaddleWidth/2) * scaleX, screenHeight / 2 + 150 * scaleY, basePaddleWidth * scaleX, basePaddleHeight * scaleY };
                player2.rect = (Rectangle){ screenWidth / 2 - (basePaddleWidth/2) * scaleX, screenHeight / 2 - 170 * scaleY, basePaddleWidth * scaleX, basePaddleHeight * scaleY };
                ball.position = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f + 135 * scaleY };
                ball.speed.x = baseBallSpeed * scaleX;
                ball.speed.y = -baseBallSpeed * scaleY;
                for (int i = 0; i < BRICK_ROWS; i++) {
                    for (int j = 0; j < BRICK_COLS; j++) {
                        bricks[i][j].rect = (Rectangle){
                            j * (baseScreenWidth / BRICK_COLS) * scaleX + (baseScreenWidth / BRICK_COLS * 0.05f) * scaleX,
                            (baseScreenHeight * 0.35f + i * (baseScreenHeight / 25.0f)) * scaleY,
                            (baseScreenWidth / BRICK_COLS * 0.9f) * scaleX,
                            (baseScreenHeight / 30.0f) * scaleY
                        };
                        bricks[i][j].active = true;
                    }
                }
                currentScreen = STATE_GAME;
            }
            break;

        case STATE_HIGHSCORE:
            ClearBackground(BLUE);
            DrawLeaderboard();
            Color backBtnColor = GRAY;
            if (CheckCollisionPointRec(GetMousePosition(), backbutton)) {
                backBtnColor = DARKGRAY;
            }
            DrawRectangleRec(backbutton, backBtnColor);
            DrawText("Geri", 20, 15, 20, BLACK);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), backbutton)) {
                currentScreen = STATE_MENU;
            }
            break;

        case STATE_GAME:
            ClearBackground(DARKGRAY);
            
            // Player 1 Movement
            if (IsKeyDown(KEY_RIGHT) && player1.rect.x < screenWidth - player1.rect.width) {
                player1.rect.x += player1.speed * GetFrameTime();
                if (player1.rect.x > screenWidth - player1.rect.width) player1.rect.x = screenWidth - player1.rect.width;
            }
            if (IsKeyDown(KEY_LEFT) && player1.rect.x > 0) {
                player1.rect.x -= player1.speed * GetFrameTime();
                if (player1.rect.x < 0) player1.rect.x = 0;
            }

            // Player 2 Movement
            if (isBotMode) {
                int k = rand() % 10 + 1, r = rand() % 10 + 1; 
                int c = screenWidth / 4; 
                double velocity; 
                if (screenWidth >= player2.rect.x + player2.rect.width && (ball.position.x >= player2.rect.x + player2.rect.width && k != 10)) { 
                    if ((r != 1 && player2.rect.x + c <= ball.position.x) || (k == 5 && player2.rect.x + c >= ball.position.x)) velocity = 1; else { velocity = 0.4; }
                    player2.rect.x += player2.speed * GetFrameTime() * velocity;
                    if (player2.rect.x > screenWidth - player2.rect.width) player2.rect.x = screenWidth - player2.rect.width;
                }
                if (player2.rect.x >= ball.position.x && k != 10) {
                    if (r != 1 && player2.rect.x >= ball.position.x + c || (k == 9 && player2.rect.x >= ball.position.x)) velocity = 1; else { velocity = 0.4; };
                    player2.rect.x -= player2.speed * GetFrameTime() * velocity;
                    if (player2.rect.x < 0) player2.rect.x = 0;
                }
            } else {
                if (IsKeyDown(KEY_D) && player2.rect.x < screenWidth - player2.rect.width) {
                    player2.rect.x += player2.speed * GetFrameTime();
                    if (player2.rect.x > screenWidth - player2.rect.width) player2.rect.x = screenWidth - player2.rect.width;
                }
                if (IsKeyDown(KEY_A) && player2.rect.x > 0) {
                    player2.rect.x -= player2.speed * GetFrameTime();
                    if (player2.rect.x < 0) player2.rect.x = 0;
                }
            }

            DrawRectangleRec(player1.rect, player1.color);
            DrawRectangleRec(player2.rect, player2.color);
            for (int i = 0; i < BRICK_ROWS; i++) {
                for (int j = 0; j < BRICK_COLS; j++) {
                    if (bricks[i][j].active) {
                        DrawRectangleRec(bricks[i][j].rect, bricks[i][j].color);
                    }
                }
            }
            DrawCircleV(ball.position, ball.radius, ball.color);
            DrawText(TextFormat("SKOR: %d", score), 20, screenHeight / 2, 20, RAYWHITE);
            
            ball.position.x += ball.speed.x * GetFrameTime();
            ball.position.y += ball.speed.y * GetFrameTime();

            // Sağ ve Sol Duvar
            if (ball.position.x >= (screenWidth - ball.radius) || ball.position.x <= ball.radius) {
                PlaySound(bam);
                ball.speed.x *= -1; 
            }

            // Üst Duvar
            if (ball.position.y <= ball.radius) {
                PlaySound(bam);
                ball.speed.y *= -1; 
            }

            // Alt Duvar
            if (ball.position.y >= (screenHeight - ball.radius)) {
                PlaySound(bam);
                ball.speed.y *= -1;
            }

            if (CheckCollisionCircleRec(ball.position, ball.radius, player1.rect)) {
                PlaySound(bam);
                ball.speed.y *= -1;     
                ball.position.y = player1.rect.y - ball.radius;
            }

            if (CheckCollisionCircleRec(ball.position, ball.radius, player2.rect)) {
                PlaySound(bam);
                ball.speed.y *= -1;
                ball.position.y = player2.rect.y + player2.rect.height + ball.radius;
            }

            for (int i = 0; i < BRICK_ROWS; i++) {
                for (int j = 0; j < BRICK_COLS; j++) {
                    if (bricks[i][j].active && CheckCollisionCircleRec(ball.position, ball.radius, bricks[i][j].rect)) {
                        PlaySound(bam);
                        ball.speed.y *= -1; 
                        bricks[i][j].active = false; 
                        score += 10;
                    }
                }
            }

            break;
        }

        if (currentScreen == STATE_GAME && IsKeyPressed(KEY_ESCAPE)) {
            SaveScore("Oyuncu", score); // Oyuncu ismini geçici olarak böyle atıyoruz
            currentScreen = STATE_MENU;
        } else if (currentScreen != STATE_GAME && currentScreen != STATE_MENU && IsKeyPressed(KEY_ESCAPE)) {
            currentScreen = STATE_MENU;
        }

        EndDrawing();
    }
    
    // Uygulama kapanırken de skoru kaydedelim ki ani kapanışlarda kaybolmasın (eğer oyundaysa)
    if (currentScreen == STATE_GAME) {
        SaveScore("Oyuncu", score);
    }

    UnloadSound(bam); 
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

void LoadScores() {
    FILE* file = fopen("scores.txt", "rb");
    if (file == NULL) {
        for (int i = 0; i < MAX_SCORES; i++) {
            strcpy(topScores[i].name, "Empty");
            topScores[i].score = 0;
        }
    }
    else {
        fread(topScores, sizeof(ScoreEntry), MAX_SCORES, file);
        fclose(file);
    }
}

void SaveScore(const char* name, int newScore) {
    if (newScore == 0) return; // 0 puanı kaydetmeye gerek yok

    for (int i = 0; i < MAX_SCORES; i++) {
        if (newScore > topScores[i].score) {
            for (int j = MAX_SCORES - 1; j > i; j--) {
                topScores[j] = topScores[j - 1];
            }
            strcpy(topScores[i].name, name);
            topScores[i].score = newScore;
            break;
        }
    }

    FILE* file = fopen("scores.txt", "wb");
    if (file != NULL) {
        fwrite(topScores, sizeof(ScoreEntry), MAX_SCORES, file);
        fclose(file);
    }
}

void DrawLeaderboard() {
    DrawText("EN YUKSEK SKORLAR", GetScreenWidth() / 2 - MeasureText("EN YUKSEK SKORLAR", 30) / 2, 80, 30, GOLD);

    for (int i = 0; i < MAX_SCORES; i++) {
        DrawText(TextFormat("%d. %s: %d", i + 1, topScores[i].name, topScores[i].score),
            GetScreenWidth() / 2 - 100, 150 + (i * 40), 25, LIGHTGRAY);
    }
}
