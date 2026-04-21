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

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    GameState currentScreen = STATE_MENU;
    int menuSelection = 1;

    Paddle player1 = { {screenWidth / 2 - 40, screenHeight / 2 + 150, 80, 20}, 500.0f, BLACK };

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
                break;
            case STATE_HIGHSCORE:
                ClearBackground(BLUE);
                break;
            case STATE_GAME:
                ClearBackground(RED);
                
                if (IsKeyDown(KEY_RIGHT) && player1.rect.x < screenWidth - player1.rect.width) {
                    player1.rect.x += player1.speed * GetFrameTime();}
                if (IsKeyDown(KEY_LEFT) && player1.rect.x > 0) {
                    player1.rect.x -= player1.speed * GetFrameTime();}
                
                DrawRectangleRec(player1.rect, player1.color);
                break;
            }
            
            if (currentScreen == STATE_MENU) {
                DrawText("PONG BREAKER ROYALE OYNA", 200, 200, 20, BLACK);
                DrawText("Oyna", screenWidth / 2 - 50, 250, 20, BLACK);
                DrawText("Skorlar", screenWidth / 2 - 50, 300, 20, BLACK);
                DrawText("Cikis", screenWidth / 2 - 50, 350, 20, BLACK);
                
                IsKeyPressed(KEY_DOWN) && menuSelection < 3 ? menuSelection++ : 0;
                IsKeyPressed(KEY_UP) && menuSelection > 1 ? menuSelection-- : 0;

                switch (menuSelection) {
                    case 1: 
                        DrawText("Oyna", screenWidth / 2 - 50, 250, 20, RED);
                        break;
                    case 2: 
                        DrawText("Skorlar", screenWidth / 2 - 50, 300, 20, RED);
                        break;
                    case 3: 
                        DrawText("Cikis", screenWidth / 2 - 50, 350, 20, RED);
                        break;
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