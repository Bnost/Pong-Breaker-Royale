#include "raylib.h"

void ToggleFullScreenWindow(int WindowWidth, int WindowHeight) 
{
    if (!IsWindowFullscreen())  //If NOT fullscreen
    {
        int monitor = GetCurrentMonitor();  //Gets currrent resolution on your monitor e.g. 1920x1080
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor)); //Window size equals monitor's size
        ToggleFullscreen(); //This will cause the fullscreen window to match the actual monitor's resolution
    }
    else
    {
        ToggleFullscreen(); //We toggeled the fullscreen to go to the regular screen size.
        SetWindowSize(WindowWidth, WindowHeight);
    }
}

int GetDisplayWidth()   //We are displaying width with this function
{
    if (IsWindowFullscreen())   //If we are at fullscreen then get monitor's width
    {
        int monitor = GetCurrentMonitor();
        return GetMonitorWidth(monitor);
    }
    else
    {
        return GetScreenWidth();    //If we are not at fullscreen than get regular screen width
    }
}

int GetDisplayHeight()  //We are displaying height with this function
{
    if (IsWindowFullscreen())   //If we are at fullscreen then get monitor's height
    {
        int monitor = GetCurrentMonitor();  
        return GetMonitorHeight(monitor);
    }
    else
    {
        return GetScreenHeight();   //If we are not at fullscreen than get regular screen height
    }
}

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int ScreenWidth = 800;  //Regular screen width
    int ScreenHeight = 450; //Regular screen height

    InitWindow(ScreenWidth, ScreenHeight, "Window");  //Set up the window
    InitAudioDevice(); // audio initiliazed
    Sound bam = LoadSound("pop.ogg");
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if (IsKeyPressed(KEY_F11))  //If you press F11 it will cause fullscreen
        {
            ToggleFullScreenWindow(ScreenWidth, ScreenHeight);
        }
        
        //Drawing
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        DrawText("Press F11 for Fullscreen!", 260, 200, 20, LIGHTGRAY);

        EndDrawing();
        if (IsKeyPressed(KEY_SPACE) {  // I will make sound appear simultaneously with collsision when we create balls
             PlaySound(bam);
            }
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
     CloseAudioDevice();
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
