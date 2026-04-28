#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "characters.h" 

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
    int owner;
} Ball;

typedef struct {
    char name[16];
    int score;
} ScoreEntry;

ScoreEntry topScores[MAX_SCORES];
void LoadScores();
void SaveScore(const char* name, int newScore);
void DrawLeaderboard();

int score1 = 0;
int score2 = 0;
bool isBotMode = false;
Character char1;
Character char2;

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

    char1 = InitCharacter(1);
    char2 = InitCharacter(2);

    Ball ball1 = { {screenWidth / 2.0f, screenHeight / 2.0f + 135 * scaleY}, {baseBallSpeed * scaleX, -baseBallSpeed * scaleY}, baseBallRadius * ((scaleX + scaleY) / 2.0f), char1.themeColor, 1 };
    Ball ball2 = { {screenWidth / 2.0f, screenHeight / 2.0f - 135 * scaleY}, {baseBallSpeed * scaleX, baseBallSpeed * scaleY}, baseBallRadius * ((scaleX + scaleY) / 2.0f), char2.themeColor, 2 };
    Paddle player1 = { {screenWidth / 2.0f - (basePaddleWidth/2.0f) * scaleX, screenHeight / 2.0f + 150 * scaleY, basePaddleWidth * scaleX, basePaddleHeight * scaleY}, basePaddleSpeed * scaleX, char1.themeColor };
    Paddle player2 = { {screenWidth / 2.0f - (basePaddleWidth/2.0f) * scaleX, screenHeight / 2.0f - 170 * scaleY, basePaddleWidth * scaleX, basePaddleHeight * scaleY}, basePaddleSpeed * scaleX, char2.themeColor };
    
    Brick bricks[BRICK_ROWS][BRICK_COLS];
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            bricks[i][j].rect = (Rectangle){
                j * (baseScreenWidth / BRICK_COLS) * scaleX + (baseScreenWidth / BRICK_COLS * 0.05f) * scaleX,
                (baseScreenHeight * 0.35f + i * (baseScreenHeight / 25.0f)) * scaleY,
                (baseScreenWidth / BRICK_COLS * 0.9f) * scaleX,
                (baseScreenHeight / 30.0f) * scaleY
            };
            if (j == 0 || j == BRICK_COLS - 1) {
                bricks[i][j].active = false;
            } else {
                bricks[i][j].active = true;
            }
            bricks[i][j].color = WHITE;
        }
    }
    Rectangle backbutton = { 10, 10, 80, 30 };

    InitWindow(screenWidth, screenHeight, "Raylib - Pong Breaker Royale");
    InitAudioDevice(); 
    Sound bam = LoadSound("Pop.ogg"); 
    
    Texture2D mikuLaserTex = LoadTexture("MikuLaser.png");

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
            ball1.position.x = (ball1.position.x / prevScreenWidth) * screenWidth;
            ball1.position.y = (ball1.position.y / prevScreenHeight) * screenHeight;
            ball1.radius = baseBallRadius * ((scaleX + scaleY) / 2.0f);
            ball1.speed.x = (ball1.speed.x > 0 ? 1 : -1) * baseBallSpeed * scaleX;
            ball1.speed.y = (ball1.speed.y > 0 ? 1 : -1) * baseBallSpeed * scaleY;

            ball2.position.x = (ball2.position.x / prevScreenWidth) * screenWidth;
            ball2.position.y = (ball2.position.y / prevScreenHeight) * screenHeight;
            ball2.radius = baseBallRadius * ((scaleX + scaleY) / 2.0f);
            ball2.speed.x = (ball2.speed.x > 0 ? 1 : -1) * baseBallSpeed * scaleX;
            ball2.speed.y = (ball2.speed.y > 0 ? 1 : -1) * baseBallSpeed * scaleY;

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
                score1 = 0;
                score2 = 0;
                char1.currentCooldown = char1.cooldownMax;
                char1.isSkillReady = false;
                char1.isSkillActive = false;
                char2.currentCooldown = char2.cooldownMax;
                char2.isSkillReady = false;
                char2.isSkillActive = false;
                player1.rect = (Rectangle){ screenWidth / 2 - (basePaddleWidth/2) * scaleX, screenHeight / 2 + 150 * scaleY, basePaddleWidth * scaleX, basePaddleHeight * scaleY };
                player2.rect = (Rectangle){ screenWidth / 2 - (basePaddleWidth/2) * scaleX, screenHeight / 2 - 170 * scaleY, basePaddleWidth * scaleX, basePaddleHeight * scaleY };
                ball1.position = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f + 135 * scaleY };
                ball1.speed.x = baseBallSpeed * scaleX;
                ball1.speed.y = -baseBallSpeed * scaleY;
                ball2.position = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f - 135 * scaleY };
                ball2.speed.x = baseBallSpeed * scaleX;
                ball2.speed.y = baseBallSpeed * scaleY;
                for (int i = 0; i < BRICK_ROWS; i++) {
                    for (int j = 0; j < BRICK_COLS; j++) {
                        bricks[i][j].rect = (Rectangle){
                            j * (baseScreenWidth / BRICK_COLS) * scaleX + (baseScreenWidth / BRICK_COLS * 0.05f) * scaleX,
                            (baseScreenHeight * 0.35f + i * (baseScreenHeight / 25.0f)) * scaleY,
                            (baseScreenWidth / BRICK_COLS * 0.9f) * scaleX,
                            (baseScreenHeight / 30.0f) * scaleY
                        };
                        if (j == 0 || j == BRICK_COLS - 1) {
                            bricks[i][j].active = false;
                        } else {
                            bricks[i][j].active = true;
                        }
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
            
            UpdateCooldown(&char1, GetFrameTime());
            UpdateCooldown(&char2, GetFrameTime());

            // Player 1 Skill
            if (IsKeyPressed(KEY_DOWN) && char1.isSkillReady) {
                char1.isSkillReady = false;
                char1.isSkillActive = true;
                char1.skillDuration = 4.0f;
                char1.skillTimer = 4.0f;
                char1.skillTickTimer = 0.5f; 
            }

            // Player 2 Skill
            if (!isBotMode && IsKeyPressed(KEY_S) && char2.isSkillReady) {
                char2.isSkillReady = false;
                char2.isSkillActive = true;
                char2.skillDuration = 4.0f;
                char2.skillTimer = 4.0f;
                char2.skillTickTimer = 0.5f;
            } else if (isBotMode && char2.isSkillReady && (rand() % 100 < 2)) {
                char2.isSkillReady = false;
                char2.isSkillActive = true;
                char2.skillDuration = 4.0f;
                char2.skillTimer = 4.0f;
                char2.skillTickTimer = 0.5f;
            }

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
                if (screenWidth >= player2.rect.x + player2.rect.width && (ball2.position.x >= player2.rect.x + player2.rect.width && k != 10)) { 
                    if ((r != 1 && player2.rect.x + c <= ball2.position.x) || (k == 5 && player2.rect.x + c >= ball2.position.x)) velocity = 1; else { velocity = 0.4; }
                    player2.rect.x += player2.speed * GetFrameTime() * velocity;
                    if (player2.rect.x > screenWidth - player2.rect.width) player2.rect.x = screenWidth - player2.rect.width;
                }
                if (player2.rect.x >= ball2.position.x && k != 10) {
                    if (r != 1 && player2.rect.x >= ball2.position.x + c || (k == 9 && player2.rect.x >= ball2.position.x)) velocity = 1; else { velocity = 0.4; };
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
            DrawCircleV(ball1.position, ball1.radius, ball1.color);
            DrawCircleV(ball2.position, ball2.radius, ball2.color);
            
            // Skill Bars
            float barWidth = 20 * scaleX;
            float barHeight = 200 * scaleY;
            float barY = screenHeight / 2.0f - barHeight / 2.0f;
            
            // Player 2 Bar (Left)
            float p2Fill = (char2.cooldownMax - char2.currentCooldown) / char2.cooldownMax;
            DrawRectangleLines(10 * scaleX, barY, barWidth, barHeight, LIGHTGRAY);
            DrawRectangle(10 * scaleX, barY + barHeight * (1.0f - p2Fill), barWidth, barHeight * p2Fill, char2.themeColor);
            
            // Player 1 Bar (Right)
            float p1Fill = (char1.cooldownMax - char1.currentCooldown) / char1.cooldownMax;
            DrawRectangleLines(screenWidth - 10 * scaleX - barWidth, barY, barWidth, barHeight, LIGHTGRAY);
            DrawRectangle(screenWidth - 10 * scaleX - barWidth, barY + barHeight * (1.0f - p1Fill), barWidth, barHeight * p1Fill, char1.themeColor);

            // Laser Logic
            Rectangle laserRect = {0};
            if (char1.isSkillActive && char1.skill == SKILL_LASER) {
                char1.frameTimer += GetFrameTime();
                if (char1.frameTimer >= (1.0f / char1.frameSpeed)) {
                    char1.frameTimer = 0.0f;
                    char1.currentFrame++;
                    if (char1.currentFrame >= char1.maxFrames) {
                        char1.currentFrame = 0;
                    }
                }

                float laserW = player1.rect.width / 2.0f; // Platformun yarısı genişliğinde
                // Lazer platformun tam üzerinden çıkıyor.
                laserRect = (Rectangle){ player1.rect.x + player1.rect.width/2 - laserW/2, 0, laserW, player1.rect.y };
                
                float frameWidth = (float)mikuLaserTex.width / char1.maxFrames;
                // Görselin altındaki ve üstündeki saydam/boş kısımları kırparak lazerin tam oturmasını sağlıyoruz
                Rectangle sourceRec = { char1.currentFrame * frameWidth, 60.0f, frameWidth, 796.0f };
                Rectangle destRec = laserRect;
                Vector2 origin = { 0.0f, 0.0f };

                BeginBlendMode(BLEND_ADDITIVE);
                DrawTexturePro(mikuLaserTex, sourceRec, destRec, origin, 0.0f, WHITE);
                EndBlendMode();
                
                if (char1.skillTickTimer >= 0.5f) {
                    bool hit = false;
                    for (int i = 0; i < BRICK_ROWS; i++) {
                        for (int j = 0; j < BRICK_COLS; j++) {
                            if (bricks[i][j].active && CheckCollisionRecs(laserRect, bricks[i][j].rect)) {
                                score1 += 10;
                                hit = true;
                            }
                        }
                    }
                    if (hit) PlaySound(bam); 
                    char1.skillTickTimer = 0.0f;
                }
            }

            DrawText(TextFormat("SKOR: %d", score2), 40 * scaleX, 20, 20, RAYWHITE);
            DrawText(TextFormat("SKOR: %d", score1), 40 * scaleX, screenHeight - 40, 20, RAYWHITE);
            
            // Ball 1 logic
            ball1.position.x += ball1.speed.x * GetFrameTime();
            ball1.position.y += ball1.speed.y * GetFrameTime();

            if (ball1.position.x >= (screenWidth - ball1.radius) || ball1.position.x <= ball1.radius) {
                PlaySound(bam);
                ball1.speed.x *= -1; 
            }
            if (ball1.position.y <= ball1.radius) {
                PlaySound(bam);
                ball1.speed.y *= -1; 
            }
            if (ball1.position.y >= (screenHeight - ball1.radius)) {
                PlaySound(bam);
                ball1.speed.y *= -1;
            }

            if (CheckCollisionCircleRec(ball1.position, ball1.radius, player1.rect)) {
                PlaySound(bam);
                ball1.speed.y *= -1;     
                ball1.position.y = player1.rect.y - ball1.radius;
            }
            if (CheckCollisionCircleRec(ball1.position, ball1.radius, player2.rect)) {
                PlaySound(bam);
                ball1.speed.y *= -1;
                ball1.position.y = player2.rect.y + player2.rect.height + ball1.radius;
            }

            // Ball 2 logic
            ball2.position.x += ball2.speed.x * GetFrameTime();
            ball2.position.y += ball2.speed.y * GetFrameTime();

            if (ball2.position.x >= (screenWidth - ball2.radius) || ball2.position.x <= ball2.radius) {
                PlaySound(bam);
                ball2.speed.x *= -1; 
            }
            if (ball2.position.y <= ball2.radius) {
                PlaySound(bam);
                ball2.speed.y *= -1; 
            }
            if (ball2.position.y >= (screenHeight - ball2.radius)) {
                PlaySound(bam);
                ball2.speed.y *= -1;
            }

            if (CheckCollisionCircleRec(ball2.position, ball2.radius, player1.rect)) {
                PlaySound(bam);
                ball2.speed.y *= -1;     
                ball2.position.y = player1.rect.y - ball2.radius;
            }
            if (CheckCollisionCircleRec(ball2.position, ball2.radius, player2.rect)) {
                PlaySound(bam);
                ball2.speed.y *= -1;
                ball2.position.y = player2.rect.y + player2.rect.height + ball2.radius;
            }

            for (int i = 0; i < BRICK_ROWS; i++) {
                for (int j = 0; j < BRICK_COLS; j++) {
                    if (bricks[i][j].active) {
                        if (CheckCollisionCircleRec(ball1.position, ball1.radius, bricks[i][j].rect)) {
                            PlaySound(bam);
                            ball1.speed.y *= -1; 
                            bricks[i][j].active = false; 
                            score1 += 10;
                        } else if (CheckCollisionCircleRec(ball2.position, ball2.radius, bricks[i][j].rect)) {
                            PlaySound(bam);
                            ball2.speed.y *= -1; 
                            bricks[i][j].active = false; 
                            score2 += 10;
                        }
                    }
                }
            }

            break;
        }

        if (currentScreen == STATE_GAME && IsKeyPressed(KEY_ESCAPE)) {
            if (score1 >= score2) SaveScore("Oyuncu 1", score1);
            else SaveScore("Oyuncu 2", score2);
            currentScreen = STATE_MENU;
        } else if (currentScreen != STATE_GAME && currentScreen != STATE_MENU && IsKeyPressed(KEY_ESCAPE)) {
            currentScreen = STATE_MENU;
        }

        EndDrawing();
    }
    
    // Uygulama kapanırken de skoru kaydedelim ki ani kapanışlarda kaybolmasın (eğer oyundaysa)
    if (currentScreen == STATE_GAME) {
        if (score1 >= score2) SaveScore("Oyuncu 1", score1);
        else SaveScore("Oyuncu 2", score2);
    }

    UnloadSound(bam); 
    UnloadTexture(mikuLaserTex);
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
