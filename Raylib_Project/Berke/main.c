#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "characters.h"
#include <math.h>

#define BRICK_ROWS 8
#define BRICK_COLS 14
#define MAX_SCORES 5

typedef enum {
    STATE_MENU,
    STATE_CHOICE,
    STATE_CHARACTER,
    STATE_HIGHSCORE,
    STATE_GAME,
    STATE_PAUSE,
} GameState;

typedef struct {
    Rectangle rect;
    float speed;
    Color color;
    float stunTimer;
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

typedef struct {
    Vector2 position;
    bool active;
    float speed;
    float size;
    Color color;
    int currentFrame;
    int maxFrames;
    float frameTimer;
    float frameSpeed;
} DrillProjectile;

DrillProjectile drill = { 0 };

typedef struct {
    Rectangle rect;
    bool active;
    float speed;
    float size;
    float targetX; // Hedef X konumu (kaçınılmaz)
    Color color;
} NeruCube;

NeruCube neruCube = { 0 };

int p1CharChoice = 1; // 1=Miku, 2=Teto, 3=Neru
int p2CharChoice = 2;
bool isP1Selecting = true; // Karakter seçiminde sıra kimin

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

    int pauseSelection = 1; // 1: Devam Et, 2: Menüye Dön
    Rectangle pauseButton = { 0 };

    char1 = InitCharacter(1);
    char2 = InitCharacter(2);

    Ball ball1 = { {screenWidth / 2.0f, screenHeight / 2.0f + 135 * scaleY}, {baseBallSpeed * scaleX, -baseBallSpeed * scaleY}, baseBallRadius * ((scaleX + scaleY) / 2.0f), char1.themeColor, 1 };
    Ball ball2 = { {screenWidth / 2.0f, screenHeight / 2.0f - 135 * scaleY}, {baseBallSpeed * scaleX, baseBallSpeed * scaleY}, baseBallRadius * ((scaleX + scaleY) / 2.0f), char2.themeColor, 2 };
    Paddle player1 = { {screenWidth / 2.0f - (basePaddleWidth / 2.0f) * scaleX, screenHeight / 2.0f + 150 * scaleY, basePaddleWidth * scaleX, basePaddleHeight * scaleY}, basePaddleSpeed * scaleX, char1.themeColor };
    Paddle player2 = { {screenWidth / 2.0f - (basePaddleWidth / 2.0f) * scaleX, screenHeight / 2.0f - 170 * scaleY, basePaddleWidth * scaleX, basePaddleHeight * scaleY}, basePaddleSpeed * scaleX, char2.themeColor };

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
            }
            else {
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
    Texture2D menuBgTex = LoadTexture("menubackground.jpg");
    Texture2D tetoDrillTex = LoadTexture("tetodrill.png");

    Music menuMusic = LoadMusicStream("triplebaksong.mp3");
    PlayMusicStream(menuMusic);

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

        UpdateMusicStream(menuMusic);
        if (currentScreen == STATE_GAME) {
            PauseMusicStream(menuMusic);
        }
        else {
            ResumeMusicStream(menuMusic);
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
            ClearBackground(BLACK);
            DrawTexturePro(menuBgTex,
                (Rectangle) {
                0, 0, menuBgTex.width, menuBgTex.height
            },
                (Rectangle) {
                0, 0, screenWidth, screenHeight
            },
                (Vector2) {
                0, 0
            }, 0.0f, WHITE);

            // Menü için RPG Maker tarzı yarı saydam arka plan kutusu
            Rectangle bgBox = { screenWidth * 0.15f, screenHeight * 0.35f, screenWidth * 0.70f, screenHeight * 0.55f };
            DrawRectangleRec(bgBox, (Color) { 0, 0, 0, 180 });

            DrawText("PLAY PONG BREAKER ROYALE", screenWidth * 0.33f, screenHeight * 0.44f, screenHeight * 0.044f, RAYWHITE);

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

            DrawText("Oyna", playRect.x, playRect.y, screenHeight * 0.044f, (menuSelection == 1) ? RED : LIGHTGRAY);
            DrawText("Skorlar", scoreRect.x, scoreRect.y, screenHeight * 0.044f, (menuSelection == 2) ? RED : LIGHTGRAY);
            DrawText("Cikis", exitRect.x, exitRect.y, screenHeight * 0.044f, (menuSelection == 3) ? RED : LIGHTGRAY);

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
            DrawTexturePro(menuBgTex,
                (Rectangle) {
                0, 0, menuBgTex.width, menuBgTex.height
            },
                (Rectangle) {
                0, 0, screenWidth, screenHeight
            },
                (Vector2) {
                0, 0
            }, 0.0f, WHITE);

            {
                Rectangle choiceBgBox = { screenWidth * 0.15f, screenHeight * 0.20f, screenWidth * 0.70f, screenHeight * 0.65f };
                DrawRectangleRec(choiceBgBox, (Color) { 0, 0, 0, 180 });
            }

            DrawText("OYUN MODU SECIN", screenWidth * 0.35f, screenHeight * 0.30f, screenHeight * 0.050f, RAYWHITE);

            Rectangle p2Rect = { screenWidth * 0.25f, screenHeight * 0.50f, screenWidth * 0.20f, screenHeight * 0.06f };
            Rectangle botRect = { screenWidth * 0.55f, screenHeight * 0.50f, screenWidth * 0.20f, screenHeight * 0.06f };

            Vector2 choiceMousePos = GetMousePosition();
            int choiceHovered = 0;
            if (CheckCollisionPointRec(choiceMousePos, p2Rect)) choiceHovered = 1;
            else if (CheckCollisionPointRec(choiceMousePos, botRect)) choiceHovered = 2;

            IsKeyPressed(KEY_RIGHT) && choiceSelection < 2 ? choiceSelection++ : 0;
            IsKeyPressed(KEY_LEFT) && choiceSelection > 1 ? choiceSelection-- : 0;

            if (choiceHovered) choiceSelection = choiceHovered;

            DrawText("2 Oyuncu", p2Rect.x, p2Rect.y, screenHeight * 0.050f, (choiceSelection == 1) ? RED : LIGHTGRAY);
            DrawText("Bot vs", botRect.x, botRect.y, screenHeight * 0.050f, (choiceSelection == 2) ? RED : LIGHTGRAY);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (choiceHovered == 1 || choiceHovered == 2) {
                    isBotMode = (choiceSelection == 2);
                    isP1Selecting = true;
                    p1CharChoice = 1;
                    p2CharChoice = 1;
                    currentScreen = STATE_CHARACTER;
                }
            }
            if (IsKeyPressed(KEY_ENTER)) {
                isBotMode = (choiceSelection == 2);
                isP1Selecting = true;
                p1CharChoice = 1;
                p2CharChoice = 1;
                currentScreen = STATE_CHARACTER;
            }
            break;

        case STATE_CHARACTER:
        {
            DrawTexturePro(menuBgTex,
                (Rectangle) {
                0, 0, menuBgTex.width, menuBgTex.height
            },
                (Rectangle) {
                0, 0, screenWidth, screenHeight
            },
                (Vector2) {
                0, 0
            }, 0.0f, WHITE);

            Rectangle charBgBox = { screenWidth * 0.10f, screenHeight * 0.10f, screenWidth * 0.80f, screenHeight * 0.80f };
            DrawRectangleRec(charBgBox, (Color) { 0, 0, 0, 180 });

            // Başlık
            const char* selectTitle = isP1Selecting ? "Player 1: Select your character" : "Player 2: Select your character";
            DrawText(selectTitle, screenWidth * 0.5f - MeasureText(selectTitle, (int)(screenHeight * 0.038f)) / 2,
                screenHeight * 0.15f, (int)(screenHeight * 0.038f), RAYWHITE);

            // 3 karakter karesi
            float boxSize = screenWidth * 0.14f;
            float boxY = screenHeight * 0.35f;
            float spacing = screenWidth * 0.20f;
            float startX = screenWidth * 0.5f - spacing;

            Color charColors[3] = { SKYBLUE, RED, YELLOW };
            const char* charNames[3] = { "Miku", "Teto", "Neru" };
            int* currentSel = isP1Selecting ? &p1CharChoice : &p2CharChoice;

            // Klavye navigasyonu (Player 1: sağ/sol, Player 2: D/A)
            if (isP1Selecting) {
                if (IsKeyPressed(KEY_RIGHT) && *currentSel < 3) (*currentSel)++;
                if (IsKeyPressed(KEY_LEFT) && *currentSel > 1) (*currentSel)--;
            }
            else {
                if (IsKeyPressed(KEY_D) && *currentSel < 3) (*currentSel)++;
                if (IsKeyPressed(KEY_A) && *currentSel > 1) (*currentSel)--;
            }

            Vector2 charMouse = GetMousePosition();
            for (int i = 0; i < 3; i++) {
                Rectangle boxRect = { startX + i * spacing - boxSize / 2.0f, boxY, boxSize, boxSize };
                DrawRectangleRec(boxRect, charColors[i]);

                // Mouse hover
                if (CheckCollisionPointRec(charMouse, boxRect)) {
                    *currentSel = i + 1;
                }

                // Seçili çerçeve
                if (*currentSel == i + 1) {
                    DrawRectangleLinesEx(boxRect, 3, WHITE);
                }

                // İsim
                int nameSize = (int)(screenHeight * 0.032f);
                DrawText(charNames[i], (int)(boxRect.x + boxSize / 2.0f - MeasureText(charNames[i], nameSize) / 2),
                    (int)(boxY + boxSize + screenHeight * 0.03f), nameSize, RAYWHITE);
            }

            // Skill açıklaması
            const char* skillDesc = "Bu skill sana oyunda yardim edecek!";
            int descSize = (int)(screenHeight * 0.030f);
            DrawText(skillDesc, screenWidth / 2 - MeasureText(skillDesc, descSize) / 2,
                (int)(screenHeight * 0.75f), descSize, LIGHTGRAY);

            // Seçim onayı
            bool charConfirmed = false;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                for (int i = 0; i < 3; i++) {
                    Rectangle boxRect = { startX + i * spacing - boxSize / 2.0f, boxY, boxSize, boxSize };
                    if (CheckCollisionPointRec(charMouse, boxRect) && *currentSel == i + 1) {
                        charConfirmed = true;
                    }
                }
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                charConfirmed = true;
            }

            if (charConfirmed) {
                if (isP1Selecting) {
                    if (isBotMode) {
                        // Bot için rastgele seçim
                        p2CharChoice = rand() % 3 + 1;
                        // Karakterleri ata
                        char1 = InitCharacter(p1CharChoice);
                        char2 = InitCharacter(p2CharChoice);
                        // Oyunu başlat
                        goto START_GAME;
                    }
                    else {
                        isP1Selecting = false;
                        p2CharChoice = 1;
                    }
                }
                else {
                    // Her iki oyuncu da seçti
                    char1 = InitCharacter(p1CharChoice);
                    char2 = InitCharacter(p2CharChoice);

                START_GAME:;
                    drill.active = false;
                    drill.currentFrame = 0;
                    drill.frameTimer = 0.0f;
                    drill.maxFrames = 4;
                    drill.frameSpeed = 15.0f;
                    neruCube.active = false;
                    score1 = 0;
                    score2 = 0;
                    char1.currentCooldown = 0;
                    char1.isSkillReady = true;
                    char1.isSkillActive = false;
                    char2.currentCooldown = 0;
                    char2.isSkillReady = true;
                    char2.isSkillActive = false;
                    player1.rect = (Rectangle){ screenWidth / 2 - (basePaddleWidth / 2) * scaleX, screenHeight / 2 + 150 * scaleY, basePaddleWidth * scaleX, basePaddleHeight * scaleY };
                    player1.color = char1.themeColor;
                    player1.stunTimer = 0.0f;
                    player2.rect = (Rectangle){ screenWidth / 2 - (basePaddleWidth / 2) * scaleX, screenHeight / 2 - 170 * scaleY, basePaddleWidth * scaleX, basePaddleHeight * scaleY };
                    player2.color = char2.themeColor;
                    player2.stunTimer = 0.0f;
                    ball1.color = char1.themeColor;
                    ball2.color = char2.themeColor;
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
                            }
                            else {
                                bricks[i][j].active = true;
                            }
                        }
                    }
                    currentScreen = STATE_GAME;
                }
            }
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

            // --- Skill Tetikleme Yardımcı Makrosu ---
           // --- Player 1 Skill (KEY_DOWN) ---
            if (IsKeyPressed(KEY_DOWN) && char1.isSkillReady) {
                char1.isSkillReady = false;
                char1.isSkillActive = true;
                char1.currentCooldown = char1.cooldownMax; // BARIN ANINDA BOŞALMASINI SAĞLAR
                char1.skillTimer = 4.0f;
                char1.skillTickTimer = 0.5f;

                if (char1.skill == SKILL_ROCKET) {
                    drill.active = true;
                    drill.position = (Vector2){ player1.rect.x + player1.rect.width / 2.0f, player1.rect.y };
                    drill.speed = -500.0f * scaleY;
                    drill.color = char1.themeColor;
                }
                else if (char1.skill == SKILL_STUN) {
                    neruCube.active = true;
                    neruCube.rect = (Rectangle){ player1.rect.x + player1.rect.width / 2.0f - 20.0f * scaleX, player1.rect.y - 20.0f * scaleX, 40.0f * scaleX, 40.0f * scaleX };
                    neruCube.speed = -600.0f * scaleY;
                    neruCube.color = char1.themeColor;
                }
                else if (char1.skill == SKILL_LASER) {
                    PlaySound(bam);
                }
            }

            // --- Player 2 Skill (KEY_S veya Bot) ---
            bool p2SkillFire = (!isBotMode && IsKeyPressed(KEY_S) && char2.isSkillReady) ||
                (isBotMode && char2.isSkillReady && (rand() % 100 < 2));

            if (p2SkillFire) {
                char2.isSkillReady = false;
                char2.isSkillActive = true;
                char2.currentCooldown = char2.cooldownMax; // PLAYER 2 İÇİN DE ANINDA BAŞLAT
                char2.skillTimer = 4.0f;
                char2.skillTickTimer = 0.5f;

                if (char2.skill == SKILL_ROCKET) {
                    drill.active = true;
                    drill.position = (Vector2){ player2.rect.x + player2.rect.width / 2.0f, player2.rect.y + player2.rect.height };
                    drill.speed = 500.0f * scaleY;
                    drill.color = char2.themeColor;
                }
                else if (char2.skill == SKILL_STUN) {
                    neruCube.active = true;
                    neruCube.rect = (Rectangle){ player2.rect.x + player2.rect.width / 2.0f - 20.0f * scaleX, player2.rect.y + player2.rect.height, 40.0f * scaleX, 40.0f * scaleX };
                    neruCube.speed = 600.0f * scaleY;
                    neruCube.color = char2.themeColor;
                }
                else if (char2.skill == SKILL_LASER) {
                    PlaySound(bam); // Miku Lazer için eksik olan tetikleyici
                }
            }

            // Player 1 Movement
            if (player1.stunTimer > 0.0f) {
                player1.stunTimer -= GetFrameTime();
                if (player1.stunTimer < 0.0f) player1.stunTimer = 0.0f;
            }
            else {
                if (IsKeyDown(KEY_RIGHT) && player1.rect.x < screenWidth - player1.rect.width) {
                    player1.rect.x += player1.speed * GetFrameTime();
                    if (player1.rect.x > screenWidth - player1.rect.width) player1.rect.x = screenWidth - player1.rect.width;
                }
                if (IsKeyDown(KEY_LEFT) && player1.rect.x > 0) {
                    player1.rect.x -= player1.speed * GetFrameTime();
                    if (player1.rect.x < 0) player1.rect.x = 0;
                }
            }

            // Player 2 Movement
            if (player2.stunTimer > 0.0f) {
                player2.stunTimer -= GetFrameTime();
                if (player2.stunTimer < 0.0f) player2.stunTimer = 0.0f;
            }
            else {
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
                }
                else {
                    if (IsKeyDown(KEY_D) && player2.rect.x < screenWidth - player2.rect.width) {
                        player2.rect.x += player2.speed * GetFrameTime();
                        if (player2.rect.x > screenWidth - player2.rect.width) player2.rect.x = screenWidth - player2.rect.width;
                    }
                    if (IsKeyDown(KEY_A) && player2.rect.x > 0) {
                        player2.rect.x -= player2.speed * GetFrameTime();
                        if (player2.rect.x < 0) player2.rect.x = 0;
                    }
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

            // Laser Logic - char1 (Player1, yukarı doğru)
            Rectangle laserRect = { 0 };
            if (char1.isSkillActive && char1.skill == SKILL_LASER) {
                char1.frameTimer += GetFrameTime();
                if (char1.frameTimer >= (1.0f / char1.frameSpeed)) {
                    char1.frameTimer = 0.0f;
                    char1.currentFrame++;
                    if (char1.currentFrame >= char1.maxFrames) char1.currentFrame = 0;
                }
                float laserW = player1.rect.width / 2.0f;
                laserRect = (Rectangle){ player1.rect.x + player1.rect.width / 2 - laserW / 2, 0, laserW, player1.rect.y };
                float frameWidth = (float)mikuLaserTex.width / char1.maxFrames;
                Rectangle sourceRec = { char1.currentFrame * frameWidth, 60.0f, frameWidth, 796.0f };
                BeginBlendMode(BLEND_ADDITIVE);
                DrawTexturePro(mikuLaserTex, sourceRec, laserRect, (Vector2) { 0, 0 }, 0.0f, WHITE);
                EndBlendMode();
                if (char1.skillTickTimer >= 0.5f) {
                    bool hit = false;
                    for (int i = 0; i < BRICK_ROWS; i++)
                        for (int j = 0; j < BRICK_COLS; j++)
                            if (bricks[i][j].active && CheckCollisionRecs(laserRect, bricks[i][j].rect)) { score1 += 10; hit = true; }
                    if (hit) PlaySound(bam);
                    char1.skillTickTimer = 0.0f;
                }
            }

            // Laser Logic - char2 (Player2, aşağı doğru)
            Rectangle laser2Rect = { 0 };
            if (char2.isSkillActive && char2.skill == SKILL_LASER) {
                char2.frameTimer += GetFrameTime();
                if (char2.frameTimer >= (1.0f / char2.frameSpeed)) {
                    char2.frameTimer = 0.0f;
                    char2.currentFrame++;
                    if (char2.currentFrame >= char2.maxFrames) char2.currentFrame = 0;
                }
                float laserW2 = player2.rect.width / 2.0f;
                // Player2 aşağıya doğru lazer: player2 platformundan ekranın altına kadar
                float laserTop = player2.rect.y + player2.rect.height;
                laser2Rect = (Rectangle){ player2.rect.x + player2.rect.width / 2 - laserW2 / 2, laserTop, laserW2, screenHeight - laserTop };
                float frameWidth2 = (float)mikuLaserTex.width / char2.maxFrames;
                Rectangle sourceRec2 = { char2.currentFrame * frameWidth2, 60.0f, frameWidth2, 796.0f };
                BeginBlendMode(BLEND_ADDITIVE);
                DrawTexturePro(mikuLaserTex, sourceRec2, laser2Rect, (Vector2) { 0, 0 }, 0.0f, WHITE);
                EndBlendMode();
                if (char2.skillTickTimer >= 0.5f) {
                    bool hit = false;
                    for (int i = 0; i < BRICK_ROWS; i++)
                        for (int j = 0; j < BRICK_COLS; j++)
                            if (bricks[i][j].active && CheckCollisionRecs(laser2Rect, bricks[i][j].rect)) { score2 += 10; hit = true; }
                    if (hit) PlaySound(bam);
                    char2.skillTickTimer = 0.0f;
                }
            }

            // Drill Logic (Teto's Skill)
            if (drill.active) {
                // Animasyon kontrolü
                drill.frameTimer += GetFrameTime();
                if (drill.frameTimer >= (1.0f / drill.frameSpeed)) {
                    drill.frameTimer = 0.0f;
                    drill.currentFrame++;
                    if (drill.currentFrame >= drill.maxFrames) {
                        drill.currentFrame = 0;
                    }
                }

                drill.position.y += drill.speed * GetFrameTime();

                Rectangle drillRect = { drill.position.x - drill.size / 2.0f, drill.position.y, drill.size, drill.size };

                // Matkabı çiz (tetodrill.png sprite animasyonu)
                float frameWidth = (float)tetoDrillTex.width / drill.maxFrames;
                // Etrafındaki siyahlıkları/boşlukları kesiyoruz (Y ekseninde 250'den başlayıp 440 birim alıyoruz)
                Rectangle sourceRec = { drill.currentFrame * frameWidth, 250.0f, frameWidth, 440.0f };
                Rectangle destRec = drillRect;
                Vector2 origin = { 0.0f, 0.0f };

                // Miku'daki gibi siyahlıkları yok etmek için
                BeginBlendMode(BLEND_ADDITIVE);
                DrawTexturePro(tetoDrillTex, sourceRec, destRec, origin, 0.0f, WHITE);
                EndBlendMode();

                // Tuğlaları kırma
                bool hitBrick = false;
                for (int i = 0; i < BRICK_ROWS; i++) {
                    for (int j = 0; j < BRICK_COLS; j++) {
                        if (bricks[i][j].active && CheckCollisionRecs(drillRect, bricks[i][j].rect)) {
                            bricks[i][j].active = false;
                            score2 += 10;
                            hitBrick = true;
                        }
                    }
                }
                if (hitBrick) PlaySound(bam);

                // Rakip platforma çarpma kontrolü
                // Çarpışma: hangi oyuncuya gidiyor? (speed > 0 = aşağı = player1'e, speed < 0 = yukarı = player2'ye)
                if (drill.speed > 0 && CheckCollisionRecs(drillRect, player1.rect)) {
                    drill.active = false;
                    char2.skillTimer = 0.0f;
                    player1.stunTimer = 2.5f;
                    PlaySound(bam);
                }
                else if (drill.speed < 0 && CheckCollisionRecs(drillRect, player2.rect)) {
                    drill.active = false;
                    char1.skillTimer = 0.0f;
                    player2.stunTimer = 2.5f;
                    PlaySound(bam);
                }

                // Ekrandan çıkma kontrolü
                if (drill.position.y > screenHeight || drill.position.y < -drill.size) {
                    drill.active = false;
                    if (drill.speed > 0) char2.skillTimer = 0.0f;
                    else char1.skillTimer = 0.0f;
                }
            }

            // Neru Cube Logic (SKILL_STUN)
            if (neruCube.active) {
                neruCube.rect.y += neruCube.speed * GetFrameTime();

                // Hedef platformu belirle ve X ekseninde takip et (kaçınılmaz)
                Rectangle* targetPaddle = (neruCube.speed > 0) ? &player1.rect : &player2.rect;
                float targetCenterX = targetPaddle->x + targetPaddle->width / 2.0f;
                float cubeCenterX = neruCube.rect.x + neruCube.size / 2.0f;
                float trackSpeed = 800.0f * scaleX * GetFrameTime(); // Çok hızlı izle
                float diff = targetCenterX - cubeCenterX;
                if (fabsf(diff) < trackSpeed)
                    neruCube.rect.x += diff;         // Tam ortala
                else
                    neruCube.rect.x += (diff > 0 ? 1.0f : -1.0f) * trackSpeed;

                // Neru küpü çiz (kaçınılmaz sarı küp)
                DrawRectangleRec(neruCube.rect, neruCube.color);
                DrawRectangleLinesEx(neruCube.rect, 2, WHITE);

                // Çarpışma: hangi yöne gidiyor?
                if (neruCube.speed > 0 && CheckCollisionRecs(neruCube.rect, player1.rect)) {
                    // Player 2'nin Neru'su Player 1'e çarptı
                    neruCube.active = false;
                    char2.skillTimer = 0.0f;
                    player1.stunTimer = 4.0f;
                    PlaySound(bam);
                }
                else if (neruCube.speed < 0 && CheckCollisionRecs(neruCube.rect, player2.rect)) {
                    // Player 1'in Neru'su Player 2'ye çarptı
                    neruCube.active = false;
                    char1.skillTimer = 0.0f;
                    player2.stunTimer = 4.0f;
                    PlaySound(bam);
                }

                // Ekrandan çıkma kontrolü (güvenlik için)
                if (neruCube.rect.y > screenHeight || neruCube.rect.y < -neruCube.size) {
                    neruCube.active = false;
                    if (neruCube.speed > 0) char2.skillTimer = 0.0f;
                    else char1.skillTimer = 0.0f;
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
                        }
                        else if (CheckCollisionCircleRec(ball2.position, ball2.radius, bricks[i][j].rect)) {
                            PlaySound(bam);
                            ball2.speed.y *= -1;
                            bricks[i][j].active = false;
                            score2 += 10;
                        }
                    }
                }
            }

            pauseButton = (Rectangle){ screenWidth - 50 * scaleX, 10 * scaleY, 40 * scaleX, 40 * scaleY };

            // Butonu Çiz
            DrawRectangleRec(pauseButton, (Color) { 0, 0, 0, 100 });
            DrawText("||", pauseButton.x + 12 * scaleX, pauseButton.y + 5 * scaleY, 30 * scaleY, RAYWHITE);

            // Tıklama veya ESC Kontrolü
            if (IsKeyPressed(KEY_ESCAPE) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), pauseButton))) {
                currentScreen = STATE_PAUSE;
            }

            break;

            case STATE_PAUSE:
            {
                // Değişkenlere güvenmek yerine doğrudan o anki render boyutlarını alıyoruz
                int currentW = GetRenderWidth();
                int currentH = GetRenderHeight();

                // Arka planı hafif karart (Tüm ekranı kaplaması için currentW/H kullanıyoruz)
                DrawRectangle(0, 0, currentW, currentH, (Color) { 0, 0, 0, 180 });

                // "GAME IS PAUSED" yazısını merkeze al
                int pauseFontSize = (int)(30 * scaleY);
                int textWidth = MeasureText("GAME IS PAUSED", pauseFontSize);
                DrawText("GAME IS PAUSED", currentW / 2 - textWidth / 2, currentH * 0.35f, pauseFontSize, RAYWHITE);

                // Buton alanlarını hesapla
                Rectangle resRect = { currentW / 2 - 100 * scaleX, currentH * 0.5f, 200 * scaleX, 40 * scaleY };
                Rectangle menRect = { currentW / 2 - 100 * scaleX, currentH * 0.6f, 200 * scaleX, 40 * scaleY };

                // Klavye Navigasyonu
                if (IsKeyPressed(KEY_DOWN)) pauseSelection = 2;
                if (IsKeyPressed(KEY_UP)) pauseSelection = 1;

                // Görsel Seçenekler (Yazıları butonların içine tam ortalıyoruz)
                int btnFontSize = (int)(25 * scaleY);

                DrawText("CONTINUE", resRect.x + (resRect.width - MeasureText("CONTINUE", btnFontSize)) / 2,
                    resRect.y, btnFontSize, (pauseSelection == 1) ? YELLOW : LIGHTGRAY);

                DrawText("MAIN MENU", menRect.x + (menRect.width - MeasureText("MAIN MENU", btnFontSize)) / 2,
                    menRect.y, btnFontSize, (pauseSelection == 2) ? YELLOW : LIGHTGRAY);

                // Seçim Onayı
                if (IsKeyPressed(KEY_ENTER) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), (pauseSelection == 1 ? resRect : menRect)))) {
                    if (pauseSelection == 1) currentScreen = STATE_GAME;
                    else {
                        if (score1 > 0 || score2 > 0) SaveScore("Son Mac", (score1 > score2 ? score1 : score2));
                        currentScreen = STATE_MENU;
                    }
                }

                // ESC'ye basınca tekrar oyuna dön
                if (IsKeyPressed(KEY_ESCAPE)) currentScreen = STATE_GAME;
            }
            break;

                // ESC'ye basınca tekrar oyuna dön
                if (IsKeyPressed(KEY_ESCAPE)) currentScreen = STATE_GAME;
                break;

        }
       
        // Diğer ekranlarda (Karakter seçimi, Skorlar vb.) ESC'ye basınca menüye döner
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (currentScreen == STATE_CHOICE || currentScreen == STATE_CHARACTER || currentScreen == STATE_HIGHSCORE) {
                currentScreen = STATE_MENU;
            }
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
    UnloadTexture(menuBgTex);
    UnloadTexture(tetoDrillTex);
    UnloadMusicStream(menuMusic);
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