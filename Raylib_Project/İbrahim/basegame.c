#include "raylib.h"

typedef enum {
    STATE_MENU,
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

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    GameState currentScreen = STATE_MENU;
    int menuSelection = 1;

    Ball ball = { {screenWidth / 2, screenHeight / 2 + 135}, {300.0f, -300.0f}, 10.0f, WHITE };
    Paddle player1 = { {screenWidth / 2 - 40, screenHeight / 2 + 150, 80, 20}, 500.0f, BLACK };
    Brick bricks[5][10];
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 10; j++) {
            bricks[i][j].rect = (Rectangle){ j * 80 + 5, i * 30 + 50, 70, 20 };
            bricks[i][j].active = true;
            bricks[i][j].color = BLACK;
        }
    }
    Rectangle backbutton = { 10, 10, 80, 30 };

    InitWindow(screenWidth, screenHeight, "Raylib - Pong Breaker");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        SetExitKey(KEY_NULL);
        BeginDrawing();

            switch (currentScreen)
            {
            case STATE_MENU:
                ClearBackground(RAYWHITE);
                player1.rect = (Rectangle){ screenWidth / 2 - 40, screenHeight / 2 + 150, 80, 20 };
                ball.position = (Vector2){ screenWidth / 2, screenHeight / 2 + 135 };
                for (int i = 0; i < 5; i++) {
                    for (int j = 0; j < 10; j++) {
                        bricks[i][j].active = true;
                    }
                }
                break;
            case STATE_HIGHSCORE:
                ClearBackground(BLUE);
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
                ClearBackground(RED);
                
                if (IsKeyDown(KEY_RIGHT) && player1.rect.x < screenWidth - player1.rect.width) {
                    player1.rect.x += player1.speed * GetFrameTime();}
                if (IsKeyDown(KEY_LEFT) && player1.rect.x > 0) {
                    player1.rect.x -= player1.speed * GetFrameTime();}
                
                DrawRectangleRec(player1.rect, player1.color);
                for (int i = 0; i < 5; i++) {
                    for (int j = 0; j < 10; j++) {
                        if (bricks[i][j].active) {
                            DrawRectangleRec(bricks[i][j].rect, bricks[i][j].color);
                        }
                    }
                }
                DrawCircleV(ball.position, ball.radius, ball.color);
                ball.position.x += ball.speed.x * GetFrameTime();
                ball.position.y += ball.speed.y * GetFrameTime();

                // Sağ ve Sol Duvar Çarpışması
                if (ball.position.x >= (GetScreenWidth() - ball.radius) || ball.position.x <= ball.radius) {
                    ball.speed.x *= -1; // X yönünü tersine çevir
                }

                // Üst Duvar Çarpışması
                if (ball.position.y <= ball.radius) {
                    ball.speed.y *= -1; // Y yönünü tersine çevir
                }

                // Alt Duvar (Şimdilik test için sekecek şekilde ayarlayalım, normalde burada can kaybedilir)
                if (ball.position.y >= (GetScreenHeight() - ball.radius)) {
                    ball.speed.y *= -1; 
                }
                if (CheckCollisionCircleRec(ball.position, ball.radius, player1.rect)) {
                    ball.speed.y *= -1;     // Topu yukarı sektir
    
                // Topun platformun içine girmesini (glitch) engellemek için 
                // topu platformun hemen üstüne iteriz:
                    ball.position.y = player1.rect.y - ball.radius; 
                }
                for (int i = 0; i < 5; i++) {
                    for (int j = 0; j < 10; j++) {
                        if (bricks[i][j].active && CheckCollisionCircleRec(ball.position, ball.radius, bricks[i][j].rect)) {
                            ball.speed.y *= -1; // Y yönünü tersine çevir
                            bricks[i][j].active = false; // Tuğlayı kır
                        }
                    }
                }

                break;
            }
            

            if (currentScreen == STATE_MENU) {
                DrawText("PONG BREAKER ROYALE OYNA", 200, 200, 20, BLACK);

                // Menü yazıları için Rectangle tanımları
                Rectangle playRect = { screenWidth / 2 - 50, 250, 120, 25 };
                Rectangle scoreRect = { screenWidth / 2 - 50, 300, 120, 25 };
                Rectangle exitRect = { screenWidth / 2 - 50, 350, 120, 25 };

                Vector2 mousePos = GetMousePosition();
                int hovered = 0;
                if (CheckCollisionPointRec(mousePos, playRect)) hovered = 1;
                else if (CheckCollisionPointRec(mousePos, scoreRect)) hovered = 2;
                else if (CheckCollisionPointRec(mousePos, exitRect)) hovered = 3;

                // Klavye ile seçim
                IsKeyPressed(KEY_DOWN) && menuSelection < 3 ? menuSelection++ : 0;
                IsKeyPressed(KEY_UP) && menuSelection > 1 ? menuSelection-- : 0;

                // Mouse ile seçim
                if (hovered) menuSelection = hovered;

                // Yazıları çiz
                DrawText("Oyna", screenWidth / 2 - 50, 250, 20, (menuSelection == 1) ? RED : BLACK);
                DrawText("Skorlar", screenWidth / 2 - 50, 300, 20, (menuSelection == 2) ? RED : BLACK);
                DrawText("Cikis", screenWidth / 2 - 50, 350, 20, (menuSelection == 3) ? RED : BLACK);

                // Mouse ile tıklama
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (hovered == 1) currentScreen = STATE_GAME;
                    else if (hovered == 2) currentScreen = STATE_HIGHSCORE;
                    else if (hovered == 3) CloseWindow();
                }
            }

            IsKeyPressed(KEY_ESCAPE) ? currentScreen = STATE_MENU : 0;

            if (IsKeyPressed(KEY_ENTER)) {
                switch (menuSelection) {
                    case 1: 
                        currentScreen = STATE_GAME;
                        break;
                    case 2: 
                        currentScreen = STATE_HIGHSCORE;
                        break;
                    case 3: 
                        CloseWindow();
                        return 0;
                }
            }

                    

        EndDrawing();
    }

    CloseWindow();

    return 0;
}