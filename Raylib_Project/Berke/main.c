#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "characters.h"
#include <math.h>

#define BRICK_ROWS 8
#define BRICK_COLS 15
#define MAX_SCORES 5
#define MAX_BALLS 10
#define MAX_BALLS_PER_PLAYER 5
#define MAX_POWERUPS 10

// Harita Düzenleri
const int mapData[5][BRICK_ROWS][BRICK_COLS] = {
    // Harita 1: "Satranç Tahtası" (Parçalı ve kaotik)
    {
        {1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 3},
        {0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 3, 0},
        {3, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1},
        {0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0},
        {0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0},
        {3, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1},
        {0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 3, 0},
        {1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 3}
    },
    // Harita 2: "Kale Duvarları" (Korunaklı yapı)
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {2, 0, 0, 0, 2, 0, 0, 3, 0, 0, 2, 0, 0, 0, 2},
        {2, 0, 3, 0, 2, 0, 1, 1, 1, 0, 2, 0, 3, 0, 2},
        {1, 1, 1, 1, 1, 4, 1, 4, 1, 4, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 4, 1, 4, 1, 4, 1, 1, 1, 1, 1},
        {2, 0, 3, 0, 2, 0, 1, 1, 1, 0, 2, 0, 3, 0, 2},
        {2, 0, 0, 0, 2, 0, 0, 3, 0, 0, 2, 0, 0, 0, 2},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    },
    // Harita 3: "Çapraz Çarpışma" (X Şekli)
    {
        {3, 0, 0, 0, 4, 0, 0, 1, 0, 0, 4, 0, 0, 0, 3},
        {0, 2, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 2, 0},
        {0, 0, 2, 0, 0, 1, 0, 4, 0, 1, 0, 0, 2, 0, 0},
        {0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0},
        {0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0},
        {0, 0, 2, 0, 0, 1, 0, 4, 0, 1, 0, 0, 2, 0, 0},
        {0, 2, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 2, 0},
        {3, 0, 0, 0, 4, 0, 0, 1, 0, 0, 4, 0, 0, 0, 3}
    },
    // Harita 4: "Elmas Madeni" (Merkez odaklı)
    {
        {0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 4, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0},
        {0, 0, 4, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
        {0, 4, 0, 0, 1, 1, 1, 3, 1, 1, 1, 0, 0, 4, 0},
        {4, 0, 0, 0, 1, 1, 1, 3, 1, 1, 1, 0, 4, 4, 4},
        {0, 4, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 4, 0},
        {0, 0, 4, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 4, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0}
    },
    // Harita 5: "Sütunlar" (Dikey koridorlar)
    {
        {1, 2, 0, 1, 2, 0, 1, 3, 1, 0, 2, 1, 0, 2, 1},
        {1, 2, 0, 1, 2, 0, 1, 3, 1, 0, 2, 1, 0, 2, 1},
        {1, 3, 4, 3, 2, 0, 1, 3, 1, 0, 2, 3, 4, 3, 1},
        {1, 4, 4, 4, 2, 0, 1, 3, 1, 0, 2, 4, 4, 4, 1},
        {1, 3, 4, 3, 2, 0, 1, 3, 1, 0, 2, 3, 4, 3, 1},
        {1, 2, 4, 1, 2, 0, 1, 3, 1, 0, 2, 1, 4, 2, 1},
        {1, 2, 0, 1, 2, 0, 1, 3, 1, 0, 2, 1, 0, 2, 1},
        {1, 2, 0, 1, 2, 0, 1, 3, 1, 0, 2, 1, 0, 2, 1}
    }
};

typedef enum {
    BRICK_NORMAL,
    BRICK_GREEN,
    BRICK_PURPLE,
    BRICK_OBSTACLE
} BrickType;

typedef enum {
    STATE_MENU,
    STATE_CHOICE,
    STATE_CHARACTER,
    STATE_MAP,
    STATE_HIGHSCORE,
    STATE_GAME,
    STATE_PAUSE,
    STATE_GAMEOVER,
    STATE_INPUT_NAME
} GameState;

typedef enum {
    PW_COOLDOWN,
    PW_POINTS,
    PW_EXTRA_BALL
} PowerUpType;

typedef struct {
    Rectangle rect;
    float speed;
    float leftLimit;  // Gidebileceği en sol
    float rightLimit; // Gidebileceği en sağ
    bool active;
} MovingObstacle;
MovingObstacle movers[2]; // Her harita için 2 adet hareketli engel

typedef struct {
    Rectangle rect;
    float speed;
    Color color;
    float stunTimer;
    int stunCurrentFrame;
    float stunFrameTimer;
} Paddle;

typedef struct {
    Rectangle rect;
    bool active;  // true ise ekranda duruyor, false ise kırılmış
    Color color;
    int type; // BrickType
    int health;
} Brick;

typedef struct {
    Vector2 position;
    float speed;
    bool active;
    int targetPlayer; // 1 or 2
    PowerUpType type;
} PowerUp;

typedef struct {
    Vector2 position;
    Vector2 speed;
    float radius;
    Color color;
    int owner;
    bool isMoving;
    float arrowAngle;
    float arrowRotationDir;
    bool active;
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
    int owner;
} DrillProjectile;

DrillProjectile drill = { 0 };

typedef struct {
    Rectangle rect;
    bool active;
    float speed;
    float size;
    float targetX; // Hedef X konumu (kaçınılmaz)
    Color color;
    int currentFrame;
    int maxFrames;
    float frameTimer;
    float frameSpeed;
} NeruCube;

NeruCube neruCube = { 0 };

int p1CharChoice = 1; // 1=Miku, 2=Teto, 3=Neru
int p2CharChoice = 2;
bool isP1Selecting = true; // Karakter seçiminde sıra kimin
int selectedMap = 0; // 0-4 arası harita seçimi

int score1 = 0;
int score2 = 0;
bool isBotMode = false;
Character char1;
Character char2;

Ball balls[MAX_BALLS];
PowerUp powerUps[MAX_POWERUPS];
Texture2D charactersTex;
Texture2D neruPhoneTex;
Texture2D stunEffectTex;
int stunCols = 3;
int stunRows = 2;
int stunMaxFrames = 6;
float stunFrameSpeed = 12.0f;

int main(void)
{

    char nameInput[16] = "\0";
    int letterCount = 0;
    int winnerScore = 0; // Kazananın skorunu burada tutacağız

    float gameTimer = 120.0f; // 120 saniye = 2 dakika

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

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Raylib - Pong Breaker Royale");
    InitAudioDevice();
    Sound bam = LoadSound("Pop.ogg");
    Sound mikuSkillSound = LoadSound("mikuskillsound.wav");
    Sound tetoSkillSound = LoadSound("tetoskillsound.wav");
    Sound neruSkillSound = LoadSound("neruskillsound.wav");
    Music mikuMusic = LoadMusicStream("mikutheme.ogg");
    Music tetoMusic = LoadMusicStream("tetoris.wav");
    Music neruMusic = LoadMusicStream("nerutheme.ogg");

    // Döngüsel çalması için ayarla (opsiyonel)
    mikuMusic.looping = true;
    tetoMusic.looping = true;
    neruMusic.looping = true;

    // O an hangi karakterin müziğinin çaldığını takip etmek için
    int currentLeadingChar = 0; // 0: Kimse, 1: Miku, 2: Teto, 3: Neru

    Texture2D mikuLaserTex = LoadTexture("MikuLaser.png");
    Texture2D paddleHairTex = LoadTexture("MkuPaddleHair.png");
    Texture2D menuBgTex = LoadTexture("menubackground.jpg");
    Texture2D pauseBgTex = LoadTexture("pausebackground.png");
    Texture2D scoreBgTex = LoadTexture("scorebackground.png");
    stunEffectTex = LoadTexture("stuneffect.png");
    // Skills transparency fix: trim near-black pixels with threshold
    Image drillImg = LoadImage("tetodrill.png");
    ImageColorReplace(&drillImg, BLACK, BLANK);
    // Also clear very dark pixels (near-black threshold)
    Color* drillPixels = LoadImageColors(drillImg);
    for (int px = 0; px < drillImg.width * drillImg.height; px++) {
        if (drillPixels[px].r < 30 && drillPixels[px].g < 30 && drillPixels[px].b < 30)
            ImageDrawPixel(&drillImg, px % drillImg.width, px / drillImg.width, BLANK);
    }
    UnloadImageColors(drillPixels);
    Texture2D tetoDrillTex = LoadTextureFromImage(drillImg);
    UnloadImage(drillImg);

    charactersTex = LoadTexture("characters.png");

    Image phoneImg = LoadImage("neruphone.png");
    ImageColorReplace(&phoneImg, BLACK, BLANK);
    // Also clear very dark pixels (near-black threshold)
    Color* phonePixels = LoadImageColors(phoneImg);
    for (int px = 0; px < phoneImg.width * phoneImg.height; px++) {
        if (phonePixels[px].r < 30 && phonePixels[px].g < 30 && phonePixels[px].b < 30)
            ImageDrawPixel(&phoneImg, px % phoneImg.width, px / phoneImg.width, BLANK);
    }
    UnloadImageColors(phonePixels);
    neruPhoneTex = LoadTextureFromImage(phoneImg);
    UnloadImage(phoneImg);

    neruCube.maxFrames = 4;
    neruCube.frameSpeed = 10.0f;

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
            int display = GetCurrentMonitor();
            if (IsWindowFullscreen()) {
                ToggleFullscreen();
                SetWindowSize(800, 450);
            }
            else {
                SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
                ToggleFullscreen();
            }
        }

        // --- MÜZİK YÖNETİM MERKEZİ ---
        UpdateMusicStream(menuMusic);

        if (currentScreen == STATE_GAME)
        {
            PauseMusicStream(menuMusic); // Oyun başlayınca menü müziğini duraklat

            // 1. Lideri ve Karakterini Belirle
            int winnerChar = 0;
            if (score1 > score2) winnerChar = p1CharChoice;
            else if (score2 > score1) winnerChar = p2CharChoice;
            else {
                // Skorlar eşitse çalan müzik değişmesin
                winnerChar = (currentLeadingChar == 0) ? p1CharChoice : currentLeadingChar;
            }

            // 2. Lider Değişimi Kontrolü (Müzik Geçişi)
            if (winnerChar != currentLeadingChar) {
                // Önce çalıyor olabilecek tüm karakter müziklerini durdur
                StopMusicStream(mikuMusic);
                StopMusicStream(tetoMusic);
                StopMusicStream(neruMusic);

                currentLeadingChar = winnerChar;

                // Yeni liderin müziğini başlat (En baştan başlar)
                if (currentLeadingChar == 1) PlayMusicStream(mikuMusic);
                if (currentLeadingChar == 2) PlayMusicStream(tetoMusic);
                if (currentLeadingChar == 3) PlayMusicStream(neruMusic);
            }

            // 3. Aktif Müziği Güncelle (Raylib için her karede şarttır)
            if (currentLeadingChar == 1) UpdateMusicStream(mikuMusic);
            else if (currentLeadingChar == 2) UpdateMusicStream(tetoMusic);
            else if (currentLeadingChar == 3) UpdateMusicStream(neruMusic);
        }
        else
        {
            // OYUN DIŞINDAKİ TÜM EKRANLARDA (Menu, Pause, Score, InputName vb.)
            ResumeMusicStream(menuMusic);

            // Tüm karakter müziklerini sustur
            StopMusicStream(mikuMusic);
            StopMusicStream(tetoMusic);
            StopMusicStream(neruMusic);

            // Lider değişkenini sıfırla ki oyun tekrar başladığında müzik tetiklenebilsin
            currentLeadingChar = 0;
        }
        // --- MÜZİK YÖNETİMİ BİTİŞ ---

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

            // 1. Ölçekleri ve ortak ölçeği (uniformScale) güncelle
            scaleX = (float)screenWidth / baseScreenWidth;
            scaleY = (float)screenHeight / baseScreenHeight;
            float uniformScale = (scaleX + scaleY) / 2.0f; // Ok ve top yarıçapı için en sağlıklısı

            // 2. Raketleri Güncelle (Base değerlerden sıfırdan hesapla)
            player1.rect.width = basePaddleWidth * scaleX;
            player1.rect.height = basePaddleHeight * scaleY;
            player1.speed = basePaddleSpeed * scaleX;
            // Raketi ekranın alt/üst oranına göre tam yerleştir (Kaymayı önler)
            player1.rect.y = (baseScreenHeight / 2.0f + 150.0f) * scaleY;
            player1.rect.x = (player1.rect.x / prevScreenWidth) * screenWidth; // Mevcut yatay konumu koru

            player2.rect.width = basePaddleWidth * scaleX;
            player2.rect.height = basePaddleHeight * scaleY;
            player2.speed = basePaddleSpeed * scaleX;
            player2.rect.y = (baseScreenHeight / 2.0f - 170.0f) * scaleY;
            player2.rect.x = (player2.rect.x / prevScreenWidth) * screenWidth;

            // 3. Topları Güncelle
            for (int i = 0; i < MAX_BALLS; i++) {
                if (balls[i].active) {
                    // Yarıçapı uniformScale ile güncelle (Ok ile uyumlu olur)
                    balls[i].radius = baseBallRadius * uniformScale;

                    // Pozisyonu yeni ekrana oranla
                    balls[i].position.x = (balls[i].position.x / prevScreenWidth) * screenWidth;
                    balls[i].position.y = (balls[i].position.y / prevScreenHeight) * screenHeight;

                    // Hız vektörlerini ölçekle
                    if (balls[i].isMoving) {
                        float dirX = (balls[i].speed.x > 0) ? 1.0f : -1.0f;
                        float dirY = (balls[i].speed.y > 0) ? 1.0f : -1.0f;
                        balls[i].speed.x = dirX * baseBallSpeed * scaleX;
                        balls[i].speed.y = dirY * baseBallSpeed * scaleY;
                    }
                }
            }

            // 4. Tuğlaları Güncelle (Zaten base değerlerden hesaplıyorsun, bu kısım iyi)
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

            DrawText("Play", playRect.x, playRect.y, screenHeight * 0.044f, (menuSelection == 1) ? RED : LIGHTGRAY);
            DrawText("Scores", scoreRect.x, scoreRect.y, screenHeight * 0.044f, (menuSelection == 2) ? RED : LIGHTGRAY);
            DrawText("Exit", exitRect.x, exitRect.y, screenHeight * 0.044f, (menuSelection == 3) ? RED : LIGHTGRAY);

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

            DrawText("Select Game Mode", screenWidth * 0.35f, screenHeight * 0.30f, screenHeight * 0.050f, RAYWHITE);

            Rectangle p2Rect = { screenWidth * 0.25f, screenHeight * 0.50f, screenWidth * 0.20f, screenHeight * 0.06f };
            Rectangle botRect = { screenWidth * 0.55f, screenHeight * 0.50f, screenWidth * 0.20f, screenHeight * 0.06f };

            Vector2 choiceMousePos = GetMousePosition();
            int choiceHovered = 0;
            if (CheckCollisionPointRec(choiceMousePos, p2Rect)) choiceHovered = 1;
            else if (CheckCollisionPointRec(choiceMousePos, botRect)) choiceHovered = 2;

            IsKeyPressed(KEY_RIGHT) && choiceSelection < 2 ? choiceSelection++ : 0;
            IsKeyPressed(KEY_LEFT) && choiceSelection > 1 ? choiceSelection-- : 0;

            if (choiceHovered) choiceSelection = choiceHovered;

            DrawText("2 Players", p2Rect.x, p2Rect.y, screenHeight * 0.050f, (choiceSelection == 1) ? RED : LIGHTGRAY);
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
            float charSpriteWidth = (float)charactersTex.width / 3.0f;
            int spriteIndexMap[3] = { 1, 0, 2 }; // Resimdeki yerleri: Teto(0), Miku(1), Neru(2) -> Ekranda: Miku, Teto, Neru

            for (int i = 0; i < 3; i++) {
                Rectangle boxRect = { startX + i * spacing - boxSize / 2.0f, boxY, boxSize, boxSize };

                // Draw character sprite normally for both players.
                float srcH = (float)charactersTex.height;
                Rectangle sourceRec = (Rectangle){ spriteIndexMap[i] * charSpriteWidth, 0, charSpriteWidth, srcH };
                DrawTexturePro(charactersTex, sourceRec, boxRect, (Vector2) { 0, 0 }, 0.0f, WHITE);

                // Mouse hover
                if (CheckCollisionPointRec(charMouse, boxRect)) {
                    *currentSel = i + 1;
                }

                // Seçili çerçeve
                if (*currentSel == i + 1) {
                    DrawRectangleLinesEx(boxRect, 8, WHITE);
                }

                // İsim
                int nameSize = (int)(screenHeight * 0.032f);
                DrawText(charNames[i], (int)(boxRect.x + boxSize / 2.0f - MeasureText(charNames[i], nameSize) / 2),
                    (int)(boxY + boxSize + screenHeight * 0.03f), nameSize, RAYWHITE);
            }

            // Skill açıklaması için slotlar
            const char* skillDesc = "";
            switch (*currentSel) {
            case 1: skillDesc = "Miku: Shoot a laser to gain points without breaking blocks!"; break;
            case 2: skillDesc = "Teto: Send your drill to break blocks and stun enemies!"; break;
            case 3: skillDesc = "Neru: Throw your phone and stun enemy no matter what!"; break;
            }
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
                        // Harita seçimine git
                        currentScreen = STATE_MAP;
                        selectedMap = 0;
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
                    currentScreen = STATE_MAP;
                    selectedMap = 0;
                }
            }
        }
        break;

    START_GAME:;
        gameTimer = 120.0f;
        drill.active = false;
        drill.currentFrame = 0;
        drill.frameTimer = 0.0f;
        drill.maxFrames = 4;
        drill.frameSpeed = 15.0f;
        neruCube.active = false;
        neruCube.currentFrame = 0;
        neruCube.frameTimer = 0.0f;
        score1 = 0;
        score2 = 0;

        // Aynı karakter seçilirse P2'yi karart
        if (p1CharChoice == p2CharChoice) {
            char2.themeColor = (Color){ (unsigned char)(char2.themeColor.r * 0.5f),
                                         (unsigned char)(char2.themeColor.g * 0.5f),
                                         (unsigned char)(char2.themeColor.b * 0.5f), 255 };
        }

        char1.currentCooldown = 0;
        char1.isSkillReady = true;
        char1.isSkillActive = false;
        char2.currentCooldown = 0;
        char2.isSkillReady = true;
        char2.isSkillActive = false;
        player1.rect = (Rectangle){ screenWidth / 2 - (basePaddleWidth / 2) * scaleX, screenHeight / 2 + 150 * scaleY, basePaddleWidth * scaleX, basePaddleHeight * scaleY };
        player1.color = char1.themeColor;
        player1.stunTimer = 0.0f;
        player1.stunCurrentFrame = 0;
        player1.stunFrameTimer = 0.0f;
        player2.rect = (Rectangle){ screenWidth / 2 - (basePaddleWidth / 2) * scaleX, screenHeight / 2 - 170 * scaleY, basePaddleWidth * scaleX, basePaddleHeight * scaleY };
        player2.color = char2.themeColor;
        player2.stunTimer = 0.0f;
        player2.stunCurrentFrame = 0;
        player2.stunFrameTimer = 0.0f;

        // Ball array temizle
        for (int i = 0; i < MAX_BALLS; i++) {
            balls[i].active = false;
            balls[i].radius = baseBallRadius * ((scaleX + scaleY) / 2.0f);
        }
        // Power-up array temizle
        for (int i = 0; i < MAX_POWERUPS; i++) powerUps[i].active = false;

        // Player 1 başlangıç topu
        balls[0].active = true;
        balls[0].owner = 1;
        balls[0].isMoving = false;
        balls[0].color = char1.themeColor;
        balls[0].arrowAngle = 90.0f;
        balls[0].arrowRotationDir = 1.0f;
        balls[0].speed = (Vector2){ 0, 0 };

        // Player 2 başlangıç topu
        balls[1].active = true;
        balls[1].owner = 2;
        balls[1].isMoving = false;
        balls[1].color = char2.themeColor;
        balls[1].arrowAngle = 270.0f;
        balls[1].arrowRotationDir = 1.0f;
        balls[1].speed = (Vector2){ 0, 0 };

        for (int i = 0; i < BRICK_ROWS; i++) {
            for (int j = 0; j < BRICK_COLS; j++) {
                bricks[i][j].rect = (Rectangle){
                    j * (screenWidth / BRICK_COLS),
                    (screenHeight * 0.35f + i * (screenHeight / 25.0f)),
                    (screenWidth / BRICK_COLS * 0.95f),
                    (screenHeight / 30.0f)
                };

                int val = mapData[selectedMap][i][j];
                if (val == 0) {
                    bricks[i][j].active = false;
                }
                else {
                    bricks[i][j].active = true;
                    bricks[i][j].health = 1;
                    if (val == 1) { bricks[i][j].type = BRICK_NORMAL;   bricks[i][j].color = WHITE; }
                    else if (val == 2) { bricks[i][j].type = BRICK_GREEN;    bricks[i][j].color = GREEN; }
                    else if (val == 3) { bricks[i][j].type = BRICK_PURPLE;   bricks[i][j].color = PURPLE; }
                    else if (val == 4) { bricks[i][j].type = BRICK_OBSTACLE; bricks[i][j].color = DARKGRAY; }

                    if (selectedMap == 4 && bricks[i][j].type != BRICK_OBSTACLE) {
                        bricks[i][j].health = 2; // 5. harita için 2 can
                    }
                }
            }
        }

        // --- HAREKETLİ ENGELLERİ BAŞLAT ---
        for (int i = 0; i < 2; i++) movers[i].active = false;

        if (selectedMap == 3) { // Harita 4: Elmas Madeni
            // --- ÜSTTEKİ ENGEL ---
            movers[0] = (MovingObstacle){
                { 0, screenHeight * 0.35f, 100 * scaleX, 15 * scaleY },
                250.0f * scaleX,    // Hız
                0,                  // Sol limit
                (float)screenWidth, // Sağ limit
                true
            };

            // --- ALTTAKİ ENGEL ---
            movers[1] = (MovingObstacle){
                { screenWidth - 100 * scaleX, screenHeight * 0.65f, 100 * scaleX, 15 * scaleY },
                -250.0f * scaleX,   // Ters yönde başlar
                0,
                (float)screenWidth,
                true
            };
        }
        else if (selectedMap == 4) { // Harita 5: Sütunlar
            // --- ÜSTTEKİ ENGEL ---
            movers[0] = (MovingObstacle){
                { 0, screenHeight * 0.28f, 110 * scaleX, 18 * scaleY },
                280.0f * scaleX,
                0,
                (float)screenWidth,
                true
            };

            // --- ALTTAKİ ENGEL ---
            movers[1] = (MovingObstacle){
                { screenWidth - 110 * scaleX, screenHeight * 0.72f, 110 * scaleX, 18 * scaleY },
                -280.0f * scaleX,
                0,
                (float)screenWidth,
                true
            };
        }

        currentScreen = STATE_GAME;
        break;

        case STATE_MAP:
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

            Rectangle mapBgBox = { screenWidth * 0.10f, screenHeight * 0.10f, screenWidth * 0.80f, screenHeight * 0.80f };
            DrawRectangleRec(mapBgBox, (Color) { 0, 0, 0, 180 });

            const char* mapTitle = "Select Your Arena";
            DrawText(mapTitle, screenWidth * 0.5f - MeasureText(mapTitle, (int)(screenHeight * 0.045f)) / 2,
                screenHeight * 0.15f, (int)(screenHeight * 0.045f), RAYWHITE);

            // 5 Harita kutusu (2 satır: 3 + 2)
            float boxW = screenWidth * 0.20f;
            float boxH = screenHeight * 0.12f;
            float startY = screenHeight * 0.30f;
            float spacingX = screenWidth * 0.25f;
            float spacingY = screenHeight * 0.20f;

            const char* mapNames[5] = { "Chessboard", "Castle Walls", "Cross Clash", "Diamond Mine", "Pillars" };
            Vector2 mouse = GetMousePosition();

            for (int i = 0; i < 5; i++) {
                float x, y;
                if (i < 3) {
                    x = screenWidth * 0.5f - spacingX + (i * spacingX) - boxW / 2.0f;
                    y = startY;
                }
                else {
                    x = screenWidth * 0.5f - spacingX * 0.5f + ((i - 3) * spacingX) - boxW / 2.0f;
                    y = startY + spacingY;
                }

                Rectangle box = { x, y, boxW, boxH };
                Color boxColor = (selectedMap == i) ? YELLOW : DARKGRAY;

                DrawRectangleRec(box, (Color) { 50, 50, 50, 200 });
                DrawRectangleLinesEx(box, (selectedMap == i ? 3 : 1), boxColor);

                // Harita ismi
                int fontSize = (int)(screenHeight * 0.025f);
                DrawText(mapNames[i], (int)(x + boxW / 2.0f - MeasureText(mapNames[i], fontSize) / 2),
                    (int)(y + boxH / 2.0f - fontSize / 2), fontSize, RAYWHITE);

                if (CheckCollisionPointRec(mouse, box)) {
                    selectedMap = i;
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) goto MAP_CONFIRMED;
                }
            }

            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) selectedMap = (selectedMap + 1) % 5;
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) selectedMap = (selectedMap + 4) % 5;
            if (IsKeyPressed(KEY_ENTER)) {
            MAP_CONFIRMED:
                goto START_GAME;
            }
        }
        break;


        case STATE_HIGHSCORE:
        {
            int currentW = GetRenderWidth();
            int currentH = GetRenderHeight();
            DrawTexturePro(scoreBgTex,
                (Rectangle) {
                0, 0, scoreBgTex.width, scoreBgTex.height
            },
                (Rectangle) {
                0, 0, currentW, currentH
            },
                (Vector2) {
                0, 0
            }, 0.0f, WHITE);

            Rectangle scoreBgBox = { currentW * 0.15f, currentH * 0.10f, currentW * 0.70f, currentH * 0.80f };
            DrawRectangleRec(scoreBgBox, (Color) { 0, 0, 0, 180 });

            DrawLeaderboard();
            Color backBtnColor = GRAY;
            if (CheckCollisionPointRec(GetMousePosition(), backbutton)) {
                backBtnColor = DARKGRAY;
            }
            DrawRectangleRec(backbutton, backBtnColor);
            DrawText("BACK", 20, 15, 20, BLACK);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), backbutton)) {
                currentScreen = STATE_MENU;
            }
        }
        break;

        case STATE_INPUT_NAME:
        {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && (letterCount < 15)) {
                    nameInput[letterCount] = (char)key;
                    nameInput[letterCount + 1] = '\0';
                    letterCount++;
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                letterCount--;
                if (letterCount < 0) letterCount = 0;
                nameInput[letterCount] = '\0';
            }

            // ENTER'a basınca asıl kayıt burada gerçekleşir
            if (IsKeyPressed(KEY_ENTER) && letterCount > 0) {
                SaveScore(nameInput, winnerScore); // İSMİ BURADA KAYDEDİYORUZ
                currentScreen = STATE_GAMEOVER;   // Sonra sonuç ekranına geçiyoruz
            }

            /// Arka plana hafif bir karartı/kutu ekleyelim (Menüdeki gibi)
            Rectangle bgBox = { screenWidth * 0.2f, screenHeight * 0.15f, screenWidth * 0.6f, screenHeight * 0.65f };
            DrawRectangleRec(bgBox, (Color) { 0, 0, 0, 200 });
            DrawRectangleLinesEx(bgBox, 2, GOLD);

            // Font boyutlarını değişkenlere atayalım (Ölçerken ve yazarken aynı olsun)
            int titleSize = (int)(35 * scaleY);
            int subSize = (int)(25 * scaleY);
            int inputFontSize = (int)(30 * scaleY);

            // Başlık - Tam Orta
            const char* titleText = "NEW HIGH SCORE!";
            DrawText(titleText, screenWidth / 2 - MeasureText(titleText, titleSize) / 2, screenHeight * 0.25f, titleSize, GOLD);

            // Skor Bilgisi
            const char* scoreText = TextFormat("Score: %d", winnerScore);
            DrawText(scoreText, screenWidth / 2 - MeasureText(scoreText, subSize) / 2, screenHeight * 0.35f, subSize, WHITE);

            // İsim Kutusu ve Etiketi
            const char* promptText = "Enter Your Name:";
            DrawText(promptText, screenWidth / 2 - MeasureText(promptText, 20 * scaleY) / 2, screenHeight * 0.45f, 20 * scaleY, GRAY);

            Rectangle inputBox = { screenWidth / 2 - 150 * scaleX, screenHeight * 0.52f, 300 * scaleX, 45 * scaleY };
            DrawRectangleRec(inputBox, RAYWHITE);
            DrawRectangleLinesEx(inputBox, 2, LIGHTGRAY);

            // Yazılan ismin kutu içinde ortalanması
            int nameWidth = MeasureText(nameInput, inputFontSize);
            DrawText(nameInput, screenWidth / 2 - nameWidth / 2, inputBox.y + (inputBox.height - inputFontSize) / 2, inputFontSize, MAROON);

            // İmleç (Yazma efekti)
            if ((int)(GetTime() * 2) % 2 == 0) { // Yanıp sönen imleç
                DrawRectangle(screenWidth / 2 + nameWidth / 2 + 5, inputBox.y + 10, 2, inputBox.height - 20, MAROON);
            }

            const char* footerText = "Press ENTER to save";
            DrawText(footerText, screenWidth / 2 - MeasureText(footerText, 15 * scaleY) / 2, screenHeight * 0.7f, 15 * scaleY, DARKGRAY);
        }
        break;

        case STATE_GAME:
            ClearBackground(DARKGRAY);

            UpdateCooldown(&char1, GetFrameTime());
            UpdateCooldown(&char2, GetFrameTime());

            // Zamanlayıcıyı azalt (Pause değilse)
            gameTimer -= GetFrameTime();

            // Süre bitti mi kontrolü
            if (gameTimer <= 0) {
                gameTimer = 0;
                // En yüksek skoru yapanı bul ve winnerScore'a ata
                winnerScore = (score1 > score2) ? score1 : score2;

                // İsim yazma hazırlığı
                nameInput[0] = '\0';
                letterCount = 0;

                currentScreen = STATE_INPUT_NAME; // Önce isim sorma ekranına git
            }

            // --- SAYAÇ ÇİZİMİ ---
            int minutes = (int)gameTimer / 60;
            int seconds = (int)gameTimer % 60;
            const char* timeText = TextFormat("%02d:%02d", minutes, seconds);
            int timeTextWidth = MeasureText(timeText, 30);
            DrawText(timeText, screenWidth / 2 - timeTextWidth / 2, 20, 30, (gameTimer < 10 ? RED : RAYWHITE));

            // Hareketli engelleri güncelle
            for (int i = 0; i < 2; i++) {
                if (movers[i].active) {
                    movers[i].rect.x += movers[i].speed * GetFrameTime();
                    if (movers[i].rect.x <= movers[i].leftLimit || movers[i].rect.x + movers[i].rect.width >= movers[i].rightLimit) {
                        movers[i].speed *= -1; // Sınıra çarpınca geri dön
                    }
                }
            }

            // --- DİNAMİK MÜZİK MANTIĞI ---
            int winnerChar = 0;
            if (score1 > score2) winnerChar = p1CharChoice;
            else if (score2 > score1) winnerChar = p2CharChoice;
            else {
                // Skorlar eşitse müzik değişmesin, eski lider çalsın
                winnerChar = currentLeadingChar;
                // Eğer oyun başıysa ve kimse önde değilse P1 başlasın dersen:
                if (winnerChar == 0) winnerChar = p1CharChoice;
            }

            // Eğer lider değiştiyse müziği değiştir
            if (winnerChar != currentLeadingChar) {
                // Hepsini durdur (temiz geçiş için)
                StopMusicStream(mikuMusic);
                StopMusicStream(tetoMusic);
                StopMusicStream(neruMusic);

                currentLeadingChar = winnerChar;

                // Yeni liderin müziğini başlat
                if (currentLeadingChar == 1) PlayMusicStream(mikuMusic);
                else if (currentLeadingChar == 2) PlayMusicStream(tetoMusic);
                else if (currentLeadingChar == 3) PlayMusicStream(neruMusic);
            }

            // Aktif müziği güncelle (Music stream olduğu için her frame güncellenmeli)
            if (currentLeadingChar == 1) UpdateMusicStream(mikuMusic);
            else if (currentLeadingChar == 2) UpdateMusicStream(tetoMusic);
            else if (currentLeadingChar == 3) UpdateMusicStream(neruMusic);

            // --- Skill Tetikleme Yardımcı Makrosu ---
           // --- Player 1 Skill (KEY_DOWN) ---
            if (IsKeyPressed(KEY_DOWN) && char1.isSkillReady) {
                char1.isSkillReady = false;
                char1.isSkillActive = true;
                char1.currentCooldown = char1.cooldownMax; // BARIN ANINDA BOŞALMASINI SAĞLAR
                char1.skillTimer = 4.0f;
                char1.skillTickTimer = 0.5f;

                if (char1.skill == SKILL_ROCKET) {
                    PlaySound(tetoSkillSound);
                    drill.active = true;
                    drill.owner = 1;
                    drill.active = true;
                    drill.size = 60.0f * scaleX;
                    drill.position = (Vector2){ player1.rect.x + player1.rect.width / 2.0f, player1.rect.y };
                    drill.speed = -500.0f * scaleY;
                    drill.color = char1.themeColor;
                }
                else if (char1.skill == SKILL_STUN) {
                    PlaySound(neruSkillSound);
                    neruCube.active = true;
                    neruCube.size = 40.0f * scaleX;
                    neruCube.rect = (Rectangle){ player1.rect.x + player1.rect.width / 2.0f - neruCube.size / 2.0f, player1.rect.y - neruCube.size, neruCube.size, neruCube.size };
                    neruCube.speed = -600.0f * scaleY;
                    neruCube.color = char1.themeColor;
                }
                else if (char1.skill == SKILL_LASER) {
                    PlaySound(mikuSkillSound);
                }
            }

            // --- Player 2 Skill (KEY_S veya Bot) ---
            bool p2SkillFire = (!isBotMode && IsKeyPressed(KEY_S) && char2.isSkillReady) ||
                (isBotMode && char2.isSkillReady && (rand() % 100 < 2));

            if (p2SkillFire) {
                char2.isSkillReady = false;
                char2.isSkillActive = true;
                char2.currentCooldown = char2.cooldownMax;
                char2.skillTimer = 4.0f;
                char2.skillTickTimer = 0.5f;

                if (char2.skill == SKILL_ROCKET) {
                    PlaySound(tetoSkillSound);
                    drill.active = true;
                    drill.owner = 2;
                    drill.size = 60.0f * scaleX;
                    drill.position = (Vector2){ player2.rect.x + player2.rect.width / 2.0f, player2.rect.y + player2.rect.height };
                    drill.speed = 500.0f * scaleY;
                    drill.color = char2.themeColor;
                }
                else if (char2.skill == SKILL_STUN) {
                    PlaySound(neruSkillSound);
                    neruCube.active = true;
                    neruCube.size = 40.0f * scaleX;
                    neruCube.rect = (Rectangle){ player2.rect.x + player2.rect.width / 2.0f - neruCube.size / 2.0f, player2.rect.y + player2.rect.height, neruCube.size, neruCube.size };
                    neruCube.speed = 600.0f * scaleY;
                    neruCube.color = char2.themeColor;
                }
                else if (char2.skill == SKILL_LASER) {
                    PlaySound(mikuSkillSound);
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
                    // Bot için en yakın topu bul
                    int closestBallIdx = -1;
                    float minDistanceY = 1000000.0f;
                    for (int i = 0; i < MAX_BALLS; i++) {
                        if (balls[i].active && balls[i].isMoving) {
                            float distY = fabsf(balls[i].position.y - player2.rect.y);
                            if (distY < minDistanceY) {
                                minDistanceY = distY;
                                closestBallIdx = i;
                            }
                        }
                    }

                    if (closestBallIdx != -1) {
                        float targetX = balls[closestBallIdx].position.x;
                        int k = rand() % 10 + 1, r = rand() % 10 + 1;
                        int c = screenWidth / 4;
                        double velocity;

                        if (player2.rect.x + player2.rect.width / 2.0f < targetX && k != 10) {
                            velocity = (r != 1) ? 1.0 : 0.4;
                            player2.rect.x += player2.speed * GetFrameTime() * velocity;
                        }
                        else if (player2.rect.x + player2.rect.width / 2.0f > targetX && k != 10) {
                            velocity = (r != 1) ? 1.0 : 0.4;
                            player2.rect.x -= player2.speed * GetFrameTime() * velocity;
                        }
                    }
                    if (player2.rect.x < 0) player2.rect.x = 0;
                    if (player2.rect.x > screenWidth - player2.rect.width) player2.rect.x = screenWidth - player2.rect.width;
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

            if (p1CharChoice == 1) BeginBlendMode(BLEND_ADDITIVE);
            if (p1CharChoice == 1) {
                float hairSrcW = paddleHairTex.width / 2.0f;
                float hairSrcH = paddleHairTex.height;
                float hairDestW = hairSrcW * scaleX;
                Rectangle leftHairRect = { player1.rect.x - hairDestW, player1.rect.y, hairDestW, player1.rect.height };
                DrawTexturePro(paddleHairTex, (Rectangle) { 0, 0, hairSrcW, hairSrcH }, leftHairRect, (Vector2) { 0, 0 }, 0.0f, WHITE);
            }
            DrawRectangleRec(player1.rect, player1.color);
            if (p1CharChoice == 1) {
                float hairSrcW = paddleHairTex.width / 2.0f;
                float hairSrcH = paddleHairTex.height;
                float hairDestW = hairSrcW * scaleX;
                Rectangle rightHairRect = { player1.rect.x + player1.rect.width, player1.rect.y, hairDestW, player1.rect.height };
                DrawTexturePro(paddleHairTex, (Rectangle) { hairSrcW, 0, hairSrcW, hairSrcH }, rightHairRect, (Vector2) { 0, 0 }, 0.0f, WHITE);
            }
            if (p1CharChoice == 1) EndBlendMode();

            if (player1.stunTimer > 0.0f) {
                player1.stunFrameTimer += GetFrameTime();
                if (player1.stunFrameTimer >= (1.0f / stunFrameSpeed)) {
                    player1.stunFrameTimer = 0.0f;
                    player1.stunCurrentFrame++;
                    if (player1.stunCurrentFrame >= stunMaxFrames) player1.stunCurrentFrame = 0;
                }
                float frameW = (float)stunEffectTex.width / stunCols;
                float frameH = (float)stunEffectTex.height / stunRows;
                int cX = player1.stunCurrentFrame % stunCols;
                int cY = player1.stunCurrentFrame / stunCols;
                Rectangle src = { cX * frameW, cY * frameH, frameW, frameH };
                // Platformun tam üstüne sabit, raket genişliği kadar küçük
                float stunSz = player1.rect.width * 1.0f;
                Rectangle dst = {
                    player1.rect.x + player1.rect.width / 2.0f - stunSz / 2.0f,
                    player1.rect.y - stunSz - 2.0f,
                    stunSz, stunSz
                };
                DrawTexturePro(stunEffectTex, src, dst, (Vector2) { 0, 0 }, 0.0f, WHITE);
            }

            if (p2CharChoice == 1) BeginBlendMode(BLEND_ADDITIVE);
            if (p2CharChoice == 1) {
                float hairSrcW = paddleHairTex.width / 2.0f;
                float hairSrcH = paddleHairTex.height;
                float hairDestW = hairSrcW * scaleX;
                Rectangle leftHairRect = { player2.rect.x - hairDestW, player2.rect.y, hairDestW, player2.rect.height };
                DrawTexturePro(paddleHairTex, (Rectangle) { 0, 0, hairSrcW, hairSrcH }, leftHairRect, (Vector2) { 0, 0 }, 0.0f, WHITE);
            }
            DrawRectangleRec(player2.rect, player2.color);
            if (p2CharChoice == 1) {
                float hairSrcW = paddleHairTex.width / 2.0f;
                float hairSrcH = paddleHairTex.height;
                float hairDestW = hairSrcW * scaleX;
                Rectangle rightHairRect = { player2.rect.x + player2.rect.width, player2.rect.y, hairDestW, player2.rect.height };
                DrawTexturePro(paddleHairTex, (Rectangle) { hairSrcW, 0, hairSrcW, hairSrcH }, rightHairRect, (Vector2) { 0, 0 }, 0.0f, WHITE);
            }
            if (p2CharChoice == 1) EndBlendMode();

            if (player2.stunTimer > 0.0f) {
                player2.stunFrameTimer += GetFrameTime();
                if (player2.stunFrameTimer >= (1.0f / stunFrameSpeed)) {
                    player2.stunFrameTimer = 0.0f;
                    player2.stunCurrentFrame++;
                    if (player2.stunCurrentFrame >= stunMaxFrames) player2.stunCurrentFrame = 0;
                }
                float frameW = (float)stunEffectTex.width / stunCols;
                float frameH = (float)stunEffectTex.height / stunRows;
                int cX = player2.stunCurrentFrame % stunCols;
                int cY = player2.stunCurrentFrame / stunCols;
                Rectangle src = { cX * frameW, cY * frameH, frameW, frameH };
                // Platformun tam üstüne sabit, raket genişliği kadar küçük
                float stunSz = player2.rect.width * 1.0f;
                Rectangle dst = {
                    player2.rect.x + player2.rect.width / 2.0f - stunSz / 2.0f,
                    player2.rect.y - stunSz - 2.0f,
                    stunSz, stunSz
                };
                DrawTexturePro(stunEffectTex, src, dst, (Vector2) { 0, 0 }, 0.0f, WHITE);
            }

            for (int i = 0; i < BRICK_ROWS; i++) {
                for (int j = 0; j < BRICK_COLS; j++) {
                    if (bricks[i][j].active) {
                        DrawRectangleRec(bricks[i][j].rect, bricks[i][j].color);
                        if (bricks[i][j].type == BRICK_OBSTACLE) {
                            // Steel cross-hatch pattern to signal "indestructible"
                            DrawRectangleLinesEx(bricks[i][j].rect, 2, GRAY);
                            DrawLine(bricks[i][j].rect.x, bricks[i][j].rect.y,
                                bricks[i][j].rect.x + bricks[i][j].rect.width,
                                bricks[i][j].rect.y + bricks[i][j].rect.height, GRAY);
                        }
                    }
                }
            }

            // --- HAREKETLİ ENGELLERİ ÇİZ ---
            for (int i = 0; i < 2; i++) {
                if (movers[i].active) {
                    DrawRectangleRec(movers[i].rect, GRAY);
                    DrawRectangleLinesEx(movers[i].rect, 2, DARKGRAY);
                    DrawLine(movers[i].rect.x, movers[i].rect.y,
                        movers[i].rect.x + movers[i].rect.width,
                        movers[i].rect.y + movers[i].rect.height, LIGHTGRAY);
                }
            }

            // Skill Bars
            float skillBarWidth = 140 * scaleX;
            float skillBarHeight = 12 * scaleY;
            float skillBarY = 55 * scaleY;

            // Player 2 Skill Bar (Sol Üst)
            float p2Fill = (char2.currentCooldown == 0) ? 1.0f : (char2.cooldownMax - char2.currentCooldown) / char2.cooldownMax;
            if (p2Fill < 0) p2Fill = 0;

            DrawRectangleLines(40 * scaleX, skillBarY, skillBarWidth, skillBarHeight, LIGHTGRAY);
            DrawRectangle(40 * scaleX, skillBarY, skillBarWidth * p2Fill, skillBarHeight, char2.themeColor);
            DrawText("SKILL", 40 * scaleX, skillBarY + skillBarHeight + 4, 10 * scaleY, LIGHTGRAY);

            // Player 1 Skill Bar (Sağ Alt)
            float p1Fill = (char1.currentCooldown == 0) ? 1.0f : (char1.cooldownMax - char1.currentCooldown) / char1.cooldownMax;
            if (p1Fill < 0) p1Fill = 0;

            float p1BarX = screenWidth - 40 * scaleX - skillBarWidth;
            float p1BarY = screenHeight - 65 * scaleY;

            DrawRectangleLines(p1BarX, p1BarY, skillBarWidth, skillBarHeight, LIGHTGRAY);
            DrawRectangle(p1BarX, p1BarY, skillBarWidth * p1Fill, skillBarHeight, char1.themeColor);
            DrawText("SKILL", p1BarX + skillBarWidth - MeasureText("SKILL", 10 * scaleY), p1BarY - 12 * scaleY, 10 * scaleY, LIGHTGRAY);

            // Laser Logic - char1 (Player1, yukarı doğru)
            Rectangle laserRect = { 0 };
            if (char1.isSkillActive && char1.skill == SKILL_LASER) {
                char1.frameTimer += GetFrameTime();
                if (char1.frameTimer >= (1.0f / char1.frameSpeed)) {
                    char1.frameTimer = 0.0f;
                    char1.currentFrame++;
                    if (char1.currentFrame >= char1.maxFrames) char1.currentFrame = 0;
                }
                float laserW = player1.rect.width * 1.5f; // Increased laser width
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
                float laserW2 = player2.rect.width * 1.5f; // Increased laser width
                // Player2 laser: from player2 paddle downward to screen bottom
                float laserTop = player2.rect.y + player2.rect.height;
                laser2Rect = (Rectangle){ player2.rect.x + player2.rect.width / 2 - laserW2 / 2, laserTop, laserW2, screenHeight - laserTop };
                float frameWidth2 = (float)mikuLaserTex.width / char2.maxFrames;
                // Flip laser texture vertically for P2 (shooting downward)
                Rectangle sourceRec2 = { char2.currentFrame * frameWidth2, 60.0f + 796.0f, frameWidth2, -796.0f };
                BeginBlendMode(BLEND_ADDITIVE);
                DrawTexturePro(mikuLaserTex, sourceRec2, laser2Rect, (Vector2) { 0, 0 }, 0.0f, WHITE);
                EndBlendMode();
                if (char2.skillTickTimer >= 0.5f) {
                    bool hit = false;
                    for (int i = 0; i < BRICK_ROWS; i++)
                        for (int j = 0; j < BRICK_COLS; j++)
                            if (bricks[i][j].active && CheckCollisionRecs(laser2Rect, bricks[i][j].rect)
                                && bricks[i][j].type != BRICK_OBSTACLE) {
                                score2 += 10; hit = true;
                            }
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
                    if (drill.currentFrame >= drill.maxFrames) drill.currentFrame = 0;
                }

                // Hareket
                drill.position.y += drill.speed * GetFrameTime();
                Rectangle drillRect = { drill.position.x - drill.size / 2.0f, drill.position.y, drill.size, drill.size };

                // Matkabı çiz: P1 yukarı gidiyorsa ters (flip), P2 aşağı gidiyorsa düz
                float frameWidth = (float)tetoDrillTex.width / drill.maxFrames;
                float texH = (float)tetoDrillTex.height;
                Rectangle sourceRec;
                if (drill.owner == 1) {
                    // Negatif height ile dikey flip (P1 yukarı atıyor)
                    sourceRec = (Rectangle){ drill.currentFrame * frameWidth, texH, frameWidth, -texH };
                }
                else {
                    sourceRec = (Rectangle){ drill.currentFrame * frameWidth, 0, frameWidth, texH };
                }

                BeginBlendMode(BLEND_ALPHA);
                DrawTexturePro(tetoDrillTex, sourceRec, drillRect, (Vector2) { 0, 0 }, 0.0f, WHITE);
                EndBlendMode();

                // TUĞLA KIRMA VE PUANLAMA MANTIĞI
                bool hitBrick = false;
                for (int i = 0; i < BRICK_ROWS; i++) {
                    for (int j = 0; j < BRICK_COLS; j++) {
                        if (bricks[i][j].active && CheckCollisionRecs(drillRect, bricks[i][j].rect)) {
                            if (bricks[i][j].type == BRICK_OBSTACLE) {
                                drill.active = false;  // Drill shatters on obstacle
                                PlaySound(bam);
                                break;
                            }
                            if (bricks[i][j].health > 1) {
                                bricks[i][j].health--;
                                bricks[i][j].color.r = (unsigned char)(bricks[i][j].color.r * 0.6f);
                                bricks[i][j].color.g = (unsigned char)(bricks[i][j].color.g * 0.6f);
                                bricks[i][j].color.b = (unsigned char)(bricks[i][j].color.b * 0.6f);
                            }
                            else {
                                bricks[i][j].active = false;
                            }
                            hitBrick = true;
                            int points = (bricks[i][j].type == BRICK_GREEN) ? 20 : 10;
                            if (drill.owner == 1) score1 += points;
                            else score2 += points;
                        }
                    }
                }
                if (hitBrick) PlaySound(bam);

                // Oyuncu Çarpışma Mantığı (Stun)
                if (drill.speed > 0 && CheckCollisionRecs(drillRect, player1.rect)) {
                    drill.active = false;
                    player1.stunTimer = 2.5f;
                    PlaySound(bam);
                }
                else if (drill.speed < 0 && CheckCollisionRecs(drillRect, player2.rect)) {
                    drill.active = false;
                    player2.stunTimer = 2.5f;
                    PlaySound(bam);
                }

                // Ekrandan çıkma kontrolü
                if (drill.position.y > screenHeight || drill.position.y < -drill.size) {
                    drill.active = false;
                }
            }

            // Neru Phone Logic (SKILL_STUN)
            if (neruCube.active) {
                // Animasyon kontrolü
                neruCube.frameTimer += GetFrameTime();
                if (neruCube.frameTimer >= (1.0f / neruCube.frameSpeed)) {
                    neruCube.frameTimer = 0.0f;
                    neruCube.currentFrame++;
                    if (neruCube.currentFrame >= neruCube.maxFrames) {
                        neruCube.currentFrame = 0;
                    }
                }

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

                // Neru telefonu çiz (neruphone.png sprite animasyonu)
                float frameWidth = (float)neruPhoneTex.width / neruCube.maxFrames;
                Rectangle sourceRec = { neruCube.currentFrame * frameWidth, 0, frameWidth, (float)neruPhoneTex.height };

                BeginBlendMode(BLEND_ALPHA);
                DrawTexturePro(neruPhoneTex, sourceRec, neruCube.rect, (Vector2) { 0, 0 }, 0.0f, WHITE);
                EndBlendMode();

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

            DrawText(TextFormat("SCORE: %d", score2), 40 * scaleX, 20, 20, RAYWHITE);
            DrawText(TextFormat("SCORE: %d", score1), 40 * scaleX, screenHeight - 40, 20, RAYWHITE);

            // --- NİŞAN ALMA AYARLARI --- 
            float uniformScale = (scaleX + scaleY) / 2.0f; // EKRAN ORANINI SABİTLE
            float rotationSpeed = 150.0f;
            float arrowLen = 60.0f * uniformScale; // scaleY yerine uniformScale kullan

            // --- BALL LOGICS (Multi-ball System) ---
            int p1BallCount = 0;
            int p2BallCount = 0;

            for (int i = 0; i < MAX_BALLS; i++) {
                if (!balls[i].active) continue;
                if (balls[i].owner == 1) p1BallCount++;
                else if (balls[i].owner == 2) p2BallCount++;

                if (!balls[i].isMoving) {
                    // Aiming state
                    Paddle* ownerPaddle = (balls[i].owner == 1) ? &player1 : &player2;

                    // Topun pozisyonunu güncel ölçeğe göre raketin merkezine bağla
                    balls[i].position.x = ownerPaddle->rect.x + ownerPaddle->rect.width / 2.0f;

                    if (balls[i].owner == 1)
                        balls[i].position.y = ownerPaddle->rect.y - balls[i].radius - 2.0f;
                    else
                        balls[i].position.y = ownerPaddle->rect.y + ownerPaddle->rect.height + balls[i].radius + 2.0f;

                    balls[i].arrowAngle += rotationSpeed * balls[i].arrowRotationDir * GetFrameTime();

                    // Açı sınırları
                    if (balls[i].owner == 1) {
                        if (balls[i].arrowAngle > 135.0f || balls[i].arrowAngle < 45.0f) balls[i].arrowRotationDir *= -1;
                    }
                    else {
                        if (balls[i].arrowAngle > 315.0f || balls[i].arrowAngle < 225.0f) balls[i].arrowRotationDir *= -1;
                    }

                    // Okun uç noktasını hesapla
                    Vector2 arrowEnd = {
                        balls[i].position.x + cosf(balls[i].arrowAngle * DEG2RAD) * arrowLen,
                        balls[i].position.y - sinf(balls[i].arrowAngle * DEG2RAD) * arrowLen
                    };

                    // ÇİZİM: Kalınlık için de uniformScale kullan (Okun yamulmasını engeller)
                    DrawLineEx(balls[i].position, arrowEnd, 4.0f * uniformScale, balls[i].color);

                    bool launchRequested = false;
                    if (balls[i].owner == 1) {
                        if (IsKeyPressed(KEY_UP)) launchRequested = true;
                    }
                    else {
                        launchRequested = (isBotMode && (rand() % 100 < 2)) || (!isBotMode && IsKeyPressed(KEY_W));
                    }

                    if (launchRequested) {
                        // Fırlatma hızını da ölçekle
                        balls[i].speed.x = cosf(balls[i].arrowAngle * DEG2RAD) * baseBallSpeed * scaleX;
                        balls[i].speed.y = -sinf(balls[i].arrowAngle * DEG2RAD) * baseBallSpeed * scaleY;
                        balls[i].isMoving = true;
                    }
                }
                else {
                    // Moving state (Hareket kodun aynı kalabilir)
                    balls[i].position.x += balls[i].speed.x * GetFrameTime();
                    balls[i].position.y += balls[i].speed.y * GetFrameTime();

                    // Wall collisions
                    if (balls[i].position.x >= (screenWidth - balls[i].radius) || balls[i].position.x <= balls[i].radius) {
                        PlaySound(bam);
                        balls[i].speed.x *= -1;
                    }
                    if (balls[i].position.y <= -balls[i].radius || balls[i].position.y >= screenHeight + balls[i].radius) {
                        balls[i].active = false; // Kenardan geçince yok olsun
                    }

                    // Hareketli engel çarpışma kontrolü
                    for (int m = 0; m < 2; m++) {
                        if (movers[m].active && CheckCollisionCircleRec(balls[i].position, balls[i].radius, movers[m].rect)) {
                            balls[i].speed.y *= -1; // Topu yansıt
                            PlaySound(bam);
                            if (balls[i].speed.y > 0) balls[i].position.y = movers[m].rect.y + movers[m].rect.height + balls[i].radius;
                            else balls[i].position.y = movers[m].rect.y - balls[i].radius;
                        }
                    }

                    // Paddle collisions & Ownership change
                    if (CheckCollisionCircleRec(balls[i].position, balls[i].radius, player1.rect)) {
                        PlaySound(bam);
                        balls[i].speed.y = -fabsf(balls[i].speed.y); // Bounce up
                        balls[i].position.y = player1.rect.y - balls[i].radius;

                        if (balls[i].owner != 1) {
                            // Player 1 takes ownership
                            if (p1BallCount < MAX_BALLS_PER_PLAYER) {
                                balls[i].owner = 1;
                                balls[i].color = char1.themeColor;
                                p1BallCount++;
                                p2BallCount--;
                            }
                            else {
                                balls[i].active = false; // Delete if limit reached
                                p2BallCount--;
                            }
                        }
                    }
                    else if (CheckCollisionCircleRec(balls[i].position, balls[i].radius, player2.rect)) {
                        PlaySound(bam);
                        balls[i].speed.y = fabsf(balls[i].speed.y); // Bounce down
                        balls[i].position.y = player2.rect.y + player2.rect.height + balls[i].radius;

                        if (balls[i].owner != 2) {
                            // Player 2 takes ownership
                            if (p2BallCount < MAX_BALLS_PER_PLAYER) {
                                balls[i].owner = 2;
                                balls[i].color = char2.themeColor;
                                p2BallCount++;
                                p1BallCount--;
                            }
                            else {
                                balls[i].active = false; // Delete if limit reached
                                p1BallCount--;
                            }
                        }
                    }

                    for (int r = 0; r < BRICK_ROWS; r++) {
                        for (int c = 0; c < BRICK_COLS; c++) {
                            if (bricks[r][c].active && CheckCollisionCircleRec(balls[i].position, balls[i].radius, bricks[r][c].rect)) {
                                PlaySound(bam);

                                Rectangle rect = bricks[r][c].rect;
                                Vector2 ballPos = balls[i].position;

                                // Topun kenarlara olan uzaklığını hesapla
                                float overlapX = 0, overlapY = 0;

                                if (ballPos.x < rect.x) overlapX = ballPos.x - rect.x; // Sol
                                else if (ballPos.x > rect.x + rect.width) overlapX = ballPos.x - (rect.x + rect.width); // Sağ

                                if (ballPos.y < rect.y) overlapY = ballPos.y - rect.y; // Üst
                                else if (ballPos.y > rect.y + rect.height) overlapY = ballPos.y - (rect.y + rect.height); // Alt

                                // Çarpışma yönüne göre hız yansıt ve topu dışarı it (Bug engelleme)
                                if (fabsf(overlapX) > fabsf(overlapY)) {
                                    balls[i].speed.x *= -1; // Yatay çarpışma
                                    if (overlapX > 0) balls[i].position.x = rect.x + rect.width + balls[i].radius;
                                    else balls[i].position.x = rect.x - balls[i].radius;
                                }
                                else {
                                    balls[i].speed.y *= -1; // Dikey çarpışma
                                    if (overlapY > 0) balls[i].position.y = rect.y + rect.height + balls[i].radius;
                                    else balls[i].position.y = rect.y - balls[i].radius;
                                }

                                // Eğer engel (indestructible) ise tuğlayı kırma, sadece sektirmiş olduk
                                if (bricks[r][c].type == BRICK_OBSTACLE) continue;

                                if (bricks[r][c].health > 1) {
                                    bricks[r][c].health--;
                                    bricks[r][c].color.r = (unsigned char)(bricks[r][c].color.r * 0.6f);
                                    bricks[r][c].color.g = (unsigned char)(bricks[r][c].color.g * 0.6f);
                                    bricks[r][c].color.b = (unsigned char)(bricks[r][c].color.b * 0.6f);
                                    continue;
                                }

                                // Normal tuğla işlemleri
                                bricks[r][c].active = false;

                                // Puan hesaplama
                                int points = (bricks[r][c].type == BRICK_GREEN) ? 20 : 10;
                                if (balls[i].owner == 1) score1 += points;
                                else score2 += points;

                                // Power-Up düşürme mantığı (kirilan mor ise)
                                if (bricks[r][c].type == BRICK_PURPLE) {
                                    for (int p = 0; p < MAX_POWERUPS; p++) {
                                        if (!powerUps[p].active) {
                                            powerUps[p].active = true;
                                            powerUps[p].position = (Vector2){ rect.x + rect.width / 2.0f, rect.y };
                                            powerUps[p].targetPlayer = balls[i].owner;
                                            powerUps[p].speed = (balls[i].owner == 1) ? 200.0f * scaleY : -200.0f * scaleY;

                                            //Rastgele tip belirleme
                                            int chance = rand() % 3;
                                            if (chance == 0) powerUps[p].type = PW_COOLDOWN;
                                            else if (chance == 1) powerUps[p].type = PW_POINTS;
                                            else powerUps[p].type = PW_EXTRA_BALL;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }

                }

                DrawCircleV(balls[i].position, balls[i].radius, balls[i].color);
            }

            // --- POWER-UP LOGIC ---
            for (int i = 0; i < MAX_POWERUPS; i++) {
                if (powerUps[i].active) {
                    powerUps[i].position.y += powerUps[i].speed * GetFrameTime();

                    // --- GÖRSELLEŞTİRME ---
                    Color pwColor = WHITE;
                    if (powerUps[i].type == PW_COOLDOWN) pwColor = PURPLE;
                    else if (powerUps[i].type == PW_POINTS) pwColor = GOLD;
                    else if (powerUps[i].type == PW_EXTRA_BALL) pwColor = SKYBLUE;

                    DrawCircleV(powerUps[i].position, 10.0f * scaleX, pwColor);
                    DrawCircleLines((int)powerUps[i].position.x, (int)powerUps[i].position.y, 10.0f * scaleX, BLACK);

                    // --- TOPLAMA (COLLISION) ---
                    Paddle* players[2] = { &player1, &player2 };
                    int* scores[2] = { &score1, &score2 };
                    Character* chars[2] = { &char1, &char2 };

                    for (int pIdx = 0; pIdx < 2; pIdx++) {
                        if (CheckCollisionCircleRec(powerUps[i].position, 10.0f * scaleX, players[pIdx]->rect)) {
                            powerUps[i].active = false;
                            PlaySound(bam);

                            switch (powerUps[i].type) {
                            case PW_COOLDOWN:
                                chars[pIdx]->currentCooldown = 0;
                                chars[pIdx]->isSkillReady = true;
                                break;

                            case PW_POINTS:
                                *(scores[pIdx]) += 100; // +100 Puan ekle
                                break;

                            case PW_EXTRA_BALL:
                                // Pasif bir top bul ve aktif et
                                for (int b = 0; b < MAX_BALLS; b++) {
                                    if (!balls[b].active) {
                                        balls[b].active = true;
                                        balls[b].owner = pIdx + 1;
                                        balls[b].isMoving = false; // Rakete yapışık başlasın
                                        balls[b].color = chars[pIdx]->themeColor;
                                        balls[b].arrowAngle = (pIdx == 0) ? 90.0f : 270.0f;
                                        balls[b].arrowRotationDir = 1.0f;
                                        balls[b].radius = baseBallRadius * ((scaleX + scaleY) / 2.0f);
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                    }

                    // Ekran sınırı kontrolü
                    if (powerUps[i].position.y < 0 || powerUps[i].position.y > screenHeight) {
                        powerUps[i].active = false;
                    }
                }
            }

            // --- SPAWN LOGIC ---
            if (p1BallCount == 0) {
                for (int i = 0; i < MAX_BALLS; i++) {
                    if (!balls[i].active) {
                        balls[i].active = true;
                        balls[i].owner = 1;
                        balls[i].isMoving = false;
                        balls[i].color = char1.themeColor;
                        balls[i].arrowAngle = 90.0f;
                        balls[i].arrowRotationDir = 1.0f;
                        balls[i].speed = (Vector2){ 0,0 };
                        break;
                    }
                }
            }
            if (p2BallCount == 0) {
                for (int i = 0; i < MAX_BALLS; i++) {
                    if (!balls[i].active) {
                        balls[i].active = true;
                        balls[i].owner = 2;
                        balls[i].isMoving = false;
                        balls[i].color = char2.themeColor;
                        balls[i].arrowAngle = 270.0f;
                        balls[i].arrowRotationDir = 1.0f;
                        balls[i].speed = (Vector2){ 0,0 };
                        break;
                    }
                }
            }

            // --- HARİTA YENİLEME MANTIĞI ---
            bool allBricksBroken = true;
            for (int i = 0; i < BRICK_ROWS; i++) {
                for (int j = 0; j < BRICK_COLS; j++) {
                    // Sadece normal tuğlaları kontrol et (Engeller/Obstacle kırılmaz olduğu için onları saymıyoruz)
                    if (bricks[i][j].active && bricks[i][j].type != BRICK_OBSTACLE) {
                        allBricksBroken = false;
                        break;
                    }
                }
                if (!allBricksBroken) break;
            }

            if (allBricksBroken) {
                // 1. Topları Sıfırla (Başlangıç durumuna getir)
                for (int i = 0; i < MAX_BALLS; i++) balls[i].active = false;

                // Player 1 başlangıç topu
                balls[0].active = true;
                balls[0].owner = 1;
                balls[0].isMoving = false;
                balls[0].color = char1.themeColor;
                balls[0].arrowAngle = 90.0f;
                balls[0].arrowRotationDir = 1.0f;

                // Player 2 başlangıç topu
                balls[1].active = true;
                balls[1].owner = 2;
                balls[1].isMoving = false;
                balls[1].color = char2.themeColor;
                balls[1].arrowAngle = 270.0f;
                balls[1].arrowRotationDir = 1.0f;

                // 2. Raketleri Başlangıç Pozisyonuna Getir (Opsiyonel, ama "yeniden başlama" hissi verir)
                player1.rect.x = screenWidth / 2.0f - player1.rect.width / 2.0f;
                player2.rect.x = screenWidth / 2.0f - player2.rect.width / 2.0f;

                // 3. Haritayı Yeniden Yükle
                for (int i = 0; i < BRICK_ROWS; i++) {
                    for (int j = 0; j < BRICK_COLS; j++) {
                        int val = mapData[selectedMap][i][j];
                        if (val != 0) {
                            bricks[i][j].active = true;
                            bricks[i][j].health = (selectedMap == 4 && val != 4) ? 2 : 1;
                            // Renkleri ve tipleri tekrar ata (Initial koddaki gibi)
                            if (val == 1) { bricks[i][j].type = BRICK_NORMAL;   bricks[i][j].color = WHITE; }
                            else if (val == 2) { bricks[i][j].type = BRICK_GREEN;    bricks[i][j].color = GREEN; }
                            else if (val == 3) { bricks[i][j].type = BRICK_PURPLE;   bricks[i][j].color = PURPLE; }
                            else if (val == 4) { bricks[i][j].type = BRICK_OBSTACLE; bricks[i][j].color = DARKGRAY; }
                        }
                    }
                }

                // Skill'leri de temizleyelim ki yeni turda bug olmasın
                drill.active = false;
                neruCube.active = false;
                char1.isSkillActive = false;
                char2.isSkillActive = false;
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
            int currentW = GetRenderWidth();
            int currentH = GetRenderHeight();
            Vector2 mousePos = GetMousePosition();

            DrawTexturePro(pauseBgTex,
                (Rectangle) {
                0, 0, pauseBgTex.width, pauseBgTex.height
            },
                (Rectangle) {
                0, 0, currentW, currentH
            },
                (Vector2) {
                0, 0
            }, 0.0f, WHITE);

            Rectangle pauseBgBox = { currentW * 0.25f, currentH * 0.25f, currentW * 0.50f, currentH * 0.50f };
            DrawRectangleRec(pauseBgBox, (Color) { 0, 0, 0, 180 });

            // Başlık
            int pauseFontSize = (int)(30 * scaleY);
            int textWidth = MeasureText("GAME IS PAUSED", pauseFontSize);
            DrawText("GAME IS PAUSED", currentW / 2 - textWidth / 2, currentH * 0.35f, pauseFontSize, RAYWHITE);

            // Buton alanları
            Rectangle resRect = { currentW / 2 - 100 * scaleX, currentH * 0.5f, 200 * scaleX, 40 * scaleY };
            Rectangle menRect = { currentW / 2 - 100 * scaleX, currentH * 0.6f, 200 * scaleX, 40 * scaleY };

            // --- FARE KONTROLÜ (HOVER) ---
            // Fare hangisinin üzerindeyse seçim o olsun
            if (CheckCollisionPointRec(mousePos, resRect)) pauseSelection = 1;
            else if (CheckCollisionPointRec(mousePos, menRect)) pauseSelection = 2;

            // Klavye Navigasyonu (Yine de çalışsın)
            if (IsKeyPressed(KEY_DOWN)) pauseSelection = 2;
            if (IsKeyPressed(KEY_UP)) pauseSelection = 1;

            // Buton Görselleri
            int btnFontSize = (int)(25 * scaleY);
            DrawText("CONTINUE", resRect.x + (resRect.width - MeasureText("CONTINUE", btnFontSize)) / 2,
                resRect.y, btnFontSize, (pauseSelection == 1) ? YELLOW : LIGHTGRAY);

            DrawText("MAIN MENU", menRect.x + (menRect.width - MeasureText("MAIN MENU", btnFontSize)) / 2,
                menRect.y, btnFontSize, (pauseSelection == 2) ? YELLOW : LIGHTGRAY);

            // --- TIKLAMA KONTROLÜ ---
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mousePos, resRect)) {
                    currentScreen = STATE_GAME;
                }
                else if (CheckCollisionPointRec(mousePos, menRect)) {
                    if (score1 > 0 || score2 > 0) SaveScore("Player 1", (score1 > score2 ? score1 : score2));
                    currentScreen = STATE_MENU;
                }
            }

            // ENTER ile onaylama kontrolü (Klavye kullanıcıları için)
            if (IsKeyPressed(KEY_ENTER)) {
                if (pauseSelection == 1) currentScreen = STATE_GAME;
                else {
                    if (score1 > 0 || score2 > 0) SaveScore("Player 2", (score1 > score2 ? score1 : score2));
                    currentScreen = STATE_MENU;
                }
            }

            // ESC ile dön
            if (IsKeyPressed(KEY_ESCAPE)) currentScreen = STATE_GAME;
        }
        break;

        case STATE_GAMEOVER:
        {
            // 1. Ekranı tamamen siyaha boyuyoruz
            ClearBackground(BLACK);

            // 2. İçerik Paneli (Siyahın üzerinde hafif bir derinlik için çok koyu gri bir kutu)
            Rectangle endBox = { screenWidth * 0.15f, screenHeight * 0.15f, screenWidth * 0.70f, screenHeight * 0.70f };
            DrawRectangleRec(endBox, (Color) { 20, 20, 20, 255 }); // Neredeyse siyah ama fark ediliyor
            DrawRectangleLinesEx(endBox, 2, RED); // Bitişi simgeleyen kırmızı çerçeve

            // Font boyutları
            int titleSize = (int)(60 * scaleY);
            int winSize = (int)(45 * scaleY);
            int scoreSize = (int)(30 * scaleY);
            int footerSize = (int)(20 * scaleY);

            // 3. Başlık: GAME OVER
            const char* title = "GAME OVER";
            DrawText(title, screenWidth / 2 - MeasureText(title, titleSize) / 2, screenHeight * 0.22f, titleSize, RED);

            // 4. Kazanan Bilgisi
            const char* winnerText;
            Color winnerColor;
            if (score1 > score2) {
                winnerText = "PLAYER 1 WINS!";
                winnerColor = char1.themeColor;
            }
            else if (score2 > score1) {
                winnerText = isBotMode ? "BOT WINS!" : "PLAYER 2 WINS!";
                winnerColor = char2.themeColor;
            }
            else {
                winnerText = "IT'S A DRAW!";
                winnerColor = WHITE;
            }
            DrawText(winnerText, screenWidth / 2 - MeasureText(winnerText, winSize) / 2, screenHeight * 0.38f, winSize, winnerColor);

            // 5. Skorlar
            const char* p1ScoreTxt = TextFormat("P1: %d", score1);
            const char* p2ScoreTxt = TextFormat("P2: %d", score2);

            DrawText(p1ScoreTxt, screenWidth * 0.35f - MeasureText(p1ScoreTxt, scoreSize) / 2, screenHeight * 0.55f, scoreSize, char1.themeColor);
            DrawText(p2ScoreTxt, screenWidth * 0.65f - MeasureText(p2ScoreTxt, scoreSize) / 2, screenHeight * 0.55f, scoreSize, char2.themeColor);

            DrawText("VS", screenWidth / 2 - MeasureText("VS", scoreSize) / 2, screenHeight * 0.55f, scoreSize, DARKGRAY);

            // 6. Kapatma Bilgisi
            const char* info = "Press ENTER to Return to Main Menu";
            DrawText(info, screenWidth / 2 - MeasureText(info, footerSize) / 2, screenHeight * 0.78f, footerSize, GRAY);

            if (IsKeyPressed(KEY_ENTER)) {
                currentScreen = STATE_MENU;
            }
        }
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

    UnloadSound(bam);
    UnloadTexture(mikuLaserTex);
    UnloadTexture(paddleHairTex);
    UnloadTexture(menuBgTex);
    UnloadTexture(tetoDrillTex);
    UnloadTexture(charactersTex);
    UnloadTexture(neruPhoneTex);
    UnloadMusicStream(menuMusic);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}


void LoadScores() {
    FILE* file = fopen("scores.txt", "rb");
    if (file == NULL) {
        for (int i = 0; i < MAX_SCORES; i++) {
            strcpy(topScores[i].name, "Empty Slot");
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
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    float sy = sh / 450.0f;
    int titleSize = (int)(40 * sy);
    int entrySize = (int)(28 * sy);
    int titleY = (int)(sh * 0.15f);
    int startY = (int)(sh * 0.28f);
    int rowSpacing = (int)(sh * 0.10f);

    DrawText("HIGH SCORES",
        sw / 2 - MeasureText("HIGH SCORES", titleSize) / 2,
        titleY, titleSize, GOLD);

    for (int i = 0; i < MAX_SCORES; i++) {
        const char* entry = TextFormat("%d. %s: %d", i + 1, topScores[i].name, topScores[i].score);
        DrawText(entry,
            sw / 2 - MeasureText(entry, entrySize) / 2,
            startY + i * rowSpacing, entrySize, LIGHTGRAY);
    }
}