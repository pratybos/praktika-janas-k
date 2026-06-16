// #include "raylib.h"

// // Define raygui implementation in only ONE source file
// #define RAYGUI_IMPLEMENTATION
// #include "raygui.h"

// // 1. Define the possible screens
// typedef enum GameScreen {
//     SCREEN_MAIN_MENU = 0,
//     SCREEN_OPTIONS,
//     SCREEN_GAMEPLAY
// } GameScreen;

// int main(void)
// {
//     // Initialization
//     const int screenWidth = 800;
//     const int screenHeight = 450;
//     InitWindow(screenWidth, screenHeight, "raygui - Screen Switching Example");

//     // 2. Initialize the starting screen
//     GameScreen currentScreen = SCREEN_MAIN_MENU;

//     SetTargetFPS(60);

//     // Main game loop
//     while (!WindowShouldClose())
//     {
//         // Update & Draw Logic
//         BeginDrawing();
//         ClearBackground(RAYWHITE);

//         // 3. State Machine to handle UI and rendering per screen
//         switch (currentScreen)
//         {
//             case SCREEN_MAIN_MENU:
//             {
//                 DrawText("MAIN MENU", 20, 20, 20, DARKGRAY);

//                 // Raygui buttons return true when clicked
//                 if (GuiButton((Rectangle){ 300, 150, 200, 40 }, "START GAME")) {
//                     currentScreen = SCREEN_GAMEPLAY; 
//                 }
//                 if (GuiButton((Rectangle){ 300, 210, 200, 40 }, "OPTIONS")) {
//                     currentScreen = SCREEN_OPTIONS;
//                 }
//                 if (GuiButton((Rectangle){ 300, 270, 200, 40 }, "EXIT")) {
//                     // Safe way to close the window
//                     break; 
//                 }
//             } break;

//             case SCREEN_OPTIONS:
//             {
//                 DrawText("OPTIONS SCREEN", 20, 20, 20, DARKGRAY);

//                 // Example option elements
//                 static float volumeValue = 0.5f;
//                 GuiSliderBar((Rectangle){ 300, 150, 200, 20 }, "Volume", NULL, &volumeValue, 0.0f, 1.0f);

//                 static bool muteValue = false;
//                 GuiCheckBox((Rectangle){ 300, 190, 20, 20 }, "Mute Audio", &muteValue);

//                 // Back button to return to Main Menu
//                 if (GuiButton((Rectangle){ 300, 300, 200, 40 }, "BACK TO MENU")) {
//                     currentScreen = SCREEN_MAIN_MENU;
//                 }
//             } break;

//             case SCREEN_GAMEPLAY:
//             {
//                 DrawText("GAMEPLAY SCREEN", 20, 20, 20, DARKGRAY);
//                 DrawText("Imagine an awesome game running here...", 250, 200, 20, LIGHTGRAY);

//                 // HUD/In-game UI button to pause or quit to menu
//                 if (GuiButton((Rectangle){ 20, 390, 150, 40 }, "QUIT TO MENU")) {
//                     currentScreen = SCREEN_MAIN_MENU;
//                 }
//             } break;

//             default: break;
//         }

//         EndDrawing();
//     }

//     // De-Initialization
//     CloseWindow();

//     return 0;
// }