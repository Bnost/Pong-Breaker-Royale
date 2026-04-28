#include "raylib.h"
#include <stdlib.h>
#include <time.h>



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
    
    int screenWidth = 800;
    int screenHeight = 450;
    GameState currentScreen = STATE_MENU;
    int menuSelection = 1;

    float baseScreenWidth = 800.0f;
    float baseScreenHeight = 450.0f;
    float baseBallRadius = 10.0f;
    float baseBallSpeed = 300.0f;
    float basePaddleWidth = 80.0f;
    float basePaddleHeight = 20.0f;
    float basePaddleSpeed = 500.0f;

    float scaleX = screenWidth / baseScreenWidth;
    float scaleY = screenHeight / baseScreenHeight;

    Ball ball = { {screenWidth / 2, screenHeight / 2 + 135 * scaleY}, {baseBallSpeed * scaleX, -baseBallSpeed * scaleY}, baseBallRadius * ((scaleX + scaleY) / 2.0f), SKYBLUE };
    Paddle player1 = { {screenWidth / 2 - (basePaddleWidth / 2) * scaleX, screenHeight / 2 + 200 * scaleY, basePaddleWidth * scaleX, basePaddleHeight * scaleY}, basePaddleSpeed * scaleX, GREEN };
    float up = screenHeight - player1.rect.y;
    Paddle player2 = { {screenWidth / 2 - (basePaddleWidth / 2) * scaleX, up , basePaddleWidth * scaleX, basePaddleHeight * scaleY }, basePaddleSpeed * scaleX, GREEN};
    Brick bricks[5][10];
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 10; j++) {
            bricks[i][j].rect = (Rectangle){ j * (baseScreenWidth / 10.0f) * scaleX + screenWidth * 0.006f,
                i * (baseScreenHeight / 15.0f) * scaleY + screenHeight * 0.11f ,
                (baseScreenWidth / 11.4f) * scaleX,
                (baseScreenHeight / 22.5f) * scaleY
            };
            bricks[i][j].active = true;
            bricks[i][j].color = WHITE;
        }
    }
    Rectangle backbutton = { 10, 10, 80, 30 };

    InitWindow(screenWidth, screenHeight, "Raylib - Pong Breaker");
    InitAudioDevice(); // initilizing sound effect
    Sound bam = LoadSound("Pop.ogg");  // defining sound effect

    SetTargetFPS(60);

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
            // Paddle'ı ekranın altına ortala
            player1.rect.width = basePaddleWidth * scaleX;
            player1.rect.height = basePaddleHeight * scaleY;
            player1.rect.x = (player1.rect.x / prevScreenWidth) * screenWidth;
            player1.rect.y = (player1.rect.y / prevScreenHeight) * screenHeight;
            player1.speed = basePaddleSpeed * scaleX;

            // 2.paddle'ı ekranın yukarısına ortala
            player2.rect.width = basePaddleWidth * scaleX;
            player2.rect.height = (basePaddleHeight) * scaleY ;
            player2.rect.x = (player2.rect.x / prevScreenWidth) * screenWidth;
            player2.rect.y =  up;
            player2.speed = basePaddleSpeed * scaleX;

            // Topun pozisyonunu ve hızını orantıla
            ball.position.x = (ball.position.x / prevScreenWidth) * screenWidth;
            ball.position.y = (ball.position.y / prevScreenHeight) * screenHeight;
            ball.radius = baseBallRadius * ((scaleX + scaleY) / 2.0f);
            ball.speed.x = (ball.speed.x > 0 ? 1 : -1) * baseBallSpeed * scaleX;
            ball.speed.y = (ball.speed.y > 0 ? 1 : -1) * baseBallSpeed * scaleY;

            // Tuğlaları yeniden ölçekle
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 10; j++) {
                    bricks[i][j].rect.x = j * (baseScreenWidth / 10.0f) * scaleX + screenWidth * 0.006f;
                    bricks[i][j].rect.y = i * (baseScreenHeight / 15.0f) * scaleY + screenHeight * 0.11f;
                    bricks[i][j].rect.width = (baseScreenWidth / 11.4f) * scaleX;
                    bricks[i][j].rect.height = (baseScreenHeight / 22.5f) * scaleY;
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
            // Menüye dönünce paddle ve topu ortala, tuğlaları resetle
            player1.rect = (Rectangle){ screenWidth / 2 - (basePaddleWidth / 2) * scaleX, screenHeight / 2 + 200 * scaleY, basePaddleWidth * scaleX, basePaddleHeight * scaleY };
            player1.speed = basePaddleSpeed * scaleX;
            player2.rect = (Rectangle){ screenWidth / 2 - (basePaddleWidth / 2) * scaleX, up, basePaddleWidth * scaleX, basePaddleHeight * scaleY };
            player2.speed = basePaddleSpeed * scaleX;

            ball.position = (Vector2){ screenWidth / 2.00f, screenHeight / 2 + 135 * scaleY };
            ball.radius = baseBallRadius * ((scaleX + scaleY) / 2.0f);
            ball.speed.x = baseBallSpeed * scaleX;
            ball.speed.y = -baseBallSpeed * scaleY;
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 10; j++) {
                    bricks[i][j].rect = (Rectangle){
                        j * (baseScreenWidth / 10.0f) * scaleX + screenWidth * 0.006f,
                        i * (baseScreenHeight / 15.0f) * scaleY + screenHeight * 0.35f,
                        (baseScreenWidth / 11.4f) * scaleX,
                        (baseScreenHeight / 22.5f) * scaleY
                    };
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
            ClearBackground(DARKGRAY);
            srand(time(NULL));
            int k = rand() % 10 + 1, r = rand() % 10 + 1; // ihtimal oluşturmak için rastgele sayılar
            int c = screenWidth / 4; // range of player2
            double velocity; // hız katsayısı
            if (IsKeyDown(KEY_RIGHT) && player1.rect.x < screenWidth - player1.rect.width) {
                player1.rect.x += player1.speed * GetFrameTime();
                if (player1.rect.x > screenWidth - player1.rect.width) player1.rect.x = screenWidth - player1.rect.width;
            }
            if (IsKeyDown(KEY_LEFT) && player1.rect.x > 0) {
                player1.rect.x -= player1.speed * GetFrameTime();
                if (player1.rect.x < 0) player1.rect.x = 0;
            }

            if ( screenWidth >= player2.rect.x + player2.rect.width && (ball.position.x>= player2.rect.x + player2.rect.width && k != 10)) { //  top yukarı paddle'a yaklaşınca % 90 olasılıkla paddle sağa kayar
                
                if (( r != 1 && player2.rect.x + c <= ball.position.x) || (k == 5 && player2.rect.x + c >= ball.position.x)) velocity = 1; else { velocity = 0.4; }
                player2.rect.x += player2.speed * GetFrameTime() * velocity;
                if (player2.rect.x > screenWidth - player2.rect.width) player2.rect.x = screenWidth - player2.rect.width;
            }
            if (player2.rect.x >= ball.position.x && k !=  10) {
                if (r != 1 && player2.rect.x >= ball.position.x + c || (k == 9 && player2.rect.x >= ball.position.x)) velocity = 1; else { velocity = 0.4; };
                player2.rect.x -= player2.speed * GetFrameTime() * velocity;
                if (player2.rect.x < 0) player2.rect.x = 0;
            }

            DrawRectangleRec(player1.rect, player1.color);
            DrawRectangleRec(player2.rect, player2.color);
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
            if (ball.position.x >= (screenWidth - ball.radius) || ball.position.x <= ball.radius) {
                PlaySound(bam);
                ball.speed.x *= -1; // X yönünü tersine çevir
            }

            // Üst Duvar Çarpışması
            if (ball.position.y <= ball.radius) {
                PlaySound(bam);
                ball.speed.y *= -1; // Y yönünü tersine çevir
            }

            // Alt Duvar (Şimdilik test için sekecek şekilde ayarlayalım, normalde burada can kaybedilir)
            if (ball.position.y >= (screenHeight - ball.radius)) {
                PlaySound(bam);
                ball.speed.y *= -1;
            }
            if (CheckCollisionCircleRec(ball.position, ball.radius, player1.rect)) {
                PlaySound(bam);
                ball.speed.y *= -1;     // Topu yukarı sektir

                // Topun platformun içine girmesini (glitch) engellemek için 
                // topu platformun hemen üstüne iteriz:
                ball.position.y = player1.rect.y - ball.radius;
            }

            if (CheckCollisionCircleRec(ball.position, ball.radius, player2.rect)) {
                PlaySound(bam);
                ball.speed.y *= -1;
                ball.position.y = player2.rect.y + ball.radius;
            }
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 10; j++) {
                    if (bricks[i][j].active && CheckCollisionCircleRec(ball.position, ball.radius, bricks[i][j].rect)) {
                        PlaySound(bam);
                        ball.speed.y *= -1; // Y yönünü tersine çevir
                        bricks[i][j].active = false; // Tuğlayı kır
                    }
                }
            }

            break;
        }



        if (currentScreen == STATE_MENU) {
            DrawText("PONG BREAKER ROYALE OYNA", screenWidth * 0.25f, screenHeight * 0.44f, screenHeight * 0.044f, BLACK);

            // Menü yazıları için Rectangle tanımları
            Rectangle playRect = { screenWidth / 2 - screenWidth * 0.062f, screenHeight * 0.56f, screenWidth * 0.15f, screenHeight * 0.055f };
            Rectangle scoreRect = { screenWidth / 2 - screenWidth * 0.062f, screenHeight * 0.67f, screenWidth * 0.15f, screenHeight * 0.055f };
            Rectangle exitRect = { screenWidth / 2 - screenWidth * 0.062f, screenHeight * 0.78f, screenWidth * 0.15f, screenHeight * 0.055f };

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
            DrawText("Oyna", playRect.x, playRect.y, screenHeight * 0.044f, (menuSelection == 1) ? RED : BLACK);
            DrawText("Skorlar", scoreRect.x, scoreRect.y, screenHeight * 0.044f, (menuSelection == 2) ? RED : BLACK);
            DrawText("Cikis", exitRect.x, exitRect.y, screenHeight * 0.044f, (menuSelection == 3) ? RED : BLACK);

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
    UnloadSound(bam); 
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
