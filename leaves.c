#include <raylib.h>
#include "rlgl.h"
#include "raymath.h"

#define TILE_SIZE 80
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define BRANCH_AMOUNT 5
#define TILES_PER_PLAYER 10
#define TURNS_PER_PLAYER 10
#define EMPTY_SPACE 0
#define GREEN_LEAF 1
#define RED_LEAF 2
#define BRANCH 3

#define ARROW_LEFT 11
#define ARROW_RIGHT 12
#define ARROW_UP 13
#define ARROW_DOWN 14
#define ARROWS_ALL 15
#define ARROWS_NONE 16

#define MAX 90
#define CENTER (MAX / 2)

int col_min, col_max, row_min, row_max;
Vector2 grid_origin;

void InitBoard(int arr[MAX][MAX], int type) {

    for (int row = 0; row < MAX; row++) {
        for (int col = 0; col < MAX; col++) {
            arr[row][col] = 0;
        }
    }
    row_min = row_max = col_min = col_max = CENTER;

    switch (type) {
        case 1:
            // Vertical line start
            row_min = CENTER - BRANCH_AMOUNT / 2;
            row_max = CENTER + BRANCH_AMOUNT / 2;
            col_min = col_max = CENTER;  

            for (int i = row_min; i <= row_max; i++) {
                arr[i][CENTER] = BRANCH;
            }
            break;

        case 2:
            // Horizontal start
            col_min = CENTER - BRANCH_AMOUNT / 2;
            col_max = CENTER + BRANCH_AMOUNT / 2;
            row_min = row_max = CENTER;  

            for (int i = col_min; i <= col_max; i++) {
                arr[CENTER][i] = BRANCH;
            }
            break;

        case 3:
            // Plus start
            arr[CENTER][CENTER] = 3;    // Center
            for (int i = 1; i < BRANCH_AMOUNT; i++) {
                switch (i % 4) {
                    case 1:         // Left
                        col_min--;
                        arr[CENTER][col_min] = BRANCH;
                        break;
                    case 2:         // UP
                        row_min--;
                        arr[row_min][CENTER] = BRANCH;
                        break;
                    case 3:         // RIGHT
                        col_max++;
                        arr[CENTER][col_max] = BRANCH;
                        break;
                    case 0:         // DOWN
                        row_max++;
                        arr[row_max][CENTER] = BRANCH;
                        break;
                }
            }
            break;

        case 4:
            // V start
            arr[CENTER][CENTER] = 3;    // Center
            for (int i = 1; i < BRANCH_AMOUNT; i++) {
                switch (i % 4) {
                    case 1:         // Left
                        col_min--;
                        arr[row_min][col_min] = BRANCH;
                        break;
                    case 2:         // Right
                        col_max++;
                        arr[row_min][col_max] = BRANCH;
                        break;
                    case 3:         // LEFT-UP
                        row_min--;
                        arr[row_min][col_min] = BRANCH;
                        break;
                    case 0:         // RIGHT-UP
                        arr[row_min][col_max] = BRANCH;
                        break;
                }
            }
            break;
    }

    int grid_width = (col_max - col_min + 3) * TILE_SIZE;
    int grid_height = (row_max - row_min + 3) * TILE_SIZE; 
    grid_origin = (Vector2) {
        (SCREEN_WIDTH - grid_width) / 2,
        (SCREEN_HEIGHT - grid_height) / 2
    };
}

void AddArrows(int arr[MAX][MAX], int mode) {

        // Clear arrows
        for (int row = row_min - 1; row <= row_max + 1; row++)
            for (int col = col_min - 1; col <= col_max + 1; col++) 
                if (arr[row][col] == ARROW_LEFT || 
                    arr[row][col] == ARROW_RIGHT || 
                    arr[row][col] == ARROW_UP || 
                    arr[row][col] == ARROW_DOWN )
                        arr[row][col] = 0;
        
        if (mode == ARROWS_NONE)
            return;

        if (mode == ARROW_DOWN || mode == ARROWS_ALL)
            for (int i = col_min; i <= col_max; i++)
                arr[row_min - 1][i] = ARROW_DOWN;
        
        if (mode == ARROW_UP || mode == ARROWS_ALL)
            for (int i = col_min; i <= col_max; i++)
                arr[row_max + 1][i] = ARROW_UP;

        if (mode == ARROW_RIGHT || mode == ARROWS_ALL)
            for (int i = row_min; i <= row_max; i++)
                arr[i][col_min - 1] = ARROW_RIGHT;

        if (mode == ARROW_LEFT || mode == ARROWS_ALL)
            for (int i = row_min; i <= row_max; i++)
                arr[i][col_max + 1] = ARROW_LEFT;
        
}

void GameMove(int arr[MAX][MAX], int turn_color, int direction, int col, int row) {
    // Tile pushing based on direction
    int i, j;
    switch (direction) {

        case ARROW_LEFT:
            i = col_max;
            while (arr[row][i] == EMPTY_SPACE || arr[row][i] > 3)
                i--;

            j = i - 1;
            while (arr[row][j] == RED_LEAF || arr[row][j] == GREEN_LEAF || arr[row][j] == BRANCH)
                j--;

            if (j < col_min)
                col_min--;

            for (; j <= i; j++)
                arr[row][j] = arr[row][j + 1];
            
            arr[row][i] = turn_color;
            break;

        case ARROW_RIGHT:
            i = col_min;
            while (arr[row][i] == EMPTY_SPACE || arr[row][i] > 3)
                i++;

            j = i + 1;
            while (arr[row][j] == RED_LEAF || arr[row][j] == GREEN_LEAF || arr[row][j] == BRANCH)
                j++;

            if (j > col_max)
                col_max++;

            for (; j >= i; j--)
                arr[row][j] = arr[row][j - 1];

            arr[row][i] = turn_color;
            break;
            
        case ARROW_UP:
            i = row_max;
            while (arr[i][col] == EMPTY_SPACE || arr[i][col] > 3)
                i--;

            j = i - 1;
            while (arr[j][col] == RED_LEAF || arr[j][col] == GREEN_LEAF || arr[j][col]== BRANCH)
                j--;

            if (j < row_min)
                row_min--;

            for (; j <= i; j++)
                arr[j][col]= arr[j + 1][col];
            
            arr[i][col] = turn_color;
            break;

        case ARROW_DOWN:
            i = row_min;
            while (arr[i][col] == EMPTY_SPACE || arr[i][col] > 3)
                i++;

            j = i + 1;
            while (arr[j][col] == RED_LEAF || arr[j][col] == GREEN_LEAF || arr[j][col]== BRANCH)
                j++;

            if (j > row_max)
                row_max++;

            for (; j >= i; j--)
                arr[j][col]= arr[j - 1][col];
            
            arr[i][col] = turn_color;
            break;
    }
}

int CheckWin(int arr[MAX][MAX]) {
    int green_pts = 0, red_pts = 0;
    
    for (int row = row_min; row <= row_max; row++) {
        for (int col = col_min; col <= col_max; col++) {
            if (arr[row][col] == GREEN_LEAF) {
                if (arr[row + 1][col] == BRANCH || 
                    arr[row - 1][col] == BRANCH || 
                    arr[row][col + 1] == BRANCH || 
                    arr[row][col - 1] == BRANCH) {
                        green_pts++;
                    }
            }
            else if (arr[row][col] == RED_LEAF) {
                if (arr[row + 1][col] == BRANCH || 
                    arr[row - 1][col] == BRANCH || 
                    arr[row][col + 1] == BRANCH || 
                    arr[row][col - 1] == BRANCH) {
                        red_pts++;
                    }
            }
        }
    }
    if (green_pts > red_pts)
        return GREEN_LEAF;
    else if (red_pts > green_pts)
        return RED_LEAF;
    else
        return 3;
        
}

int main(void) {
    int board[MAX][MAX];    
    InitBoard(board, 1);
    AddArrows(board, ARROWS_ALL);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Leaves");

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    SetTargetFPS(60);     

    Image branch_image = LoadImage("assets/branch.png");
    Image green_LEAF_image = LoadImage("assets/green.png");
    Image red_LEAF_image = LoadImage("assets/red.png");

    ImageResize(&branch_image, TILE_SIZE, TILE_SIZE);
    ImageResize(&green_LEAF_image, TILE_SIZE, TILE_SIZE);
    ImageResize(&red_LEAF_image, TILE_SIZE, TILE_SIZE);

    Texture2D branch_texture = LoadTextureFromImage(branch_image);
    Texture2D green_LEAF_texture = LoadTextureFromImage(green_LEAF_image);
    Texture2D red_LEAF_texture = LoadTextureFromImage(red_LEAF_image);

    Image arrow_image = LoadImage("assets/arrow.png");
    ImageResize(&arrow_image, TILE_SIZE, TILE_SIZE);

    Texture2D arrow_up_texture = LoadTextureFromImage(arrow_image);
    ImageRotate(&arrow_image, 90);
    Texture2D arrow_right_texture = LoadTextureFromImage(arrow_image);
    ImageRotate(&arrow_image, 90);
    Texture2D arrow_down_texture = LoadTextureFromImage(arrow_image);
    ImageRotate(&arrow_image, 90);
    Texture2D arrow_left_texture = LoadTextureFromImage(arrow_image);

    Vector2 mouse = { 0, 0 };

    int selected_col = 0, selected_row = 0;
    int turn_color = GREEN_LEAF;
    int turn_stage = 1;
    int turns_remaining[3] = {0, TURNS_PER_PLAYER, TURNS_PER_PLAYER};
    bool game_finished = false;
    int winner = 0;


    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_ONE)) {
            InitBoard(board, 1);
            AddArrows(board, ARROWS_ALL);

            turn_color = GREEN_LEAF;
            turn_stage = 1;
            turns_remaining[GREEN_LEAF] = TURNS_PER_PLAYER;
            turns_remaining[RED_LEAF] = TURNS_PER_PLAYER;
            game_finished = false;
            winner = 0;
        }
        if (IsKeyPressed(KEY_TWO)) {
            InitBoard(board, 2);
            AddArrows(board, ARROWS_ALL);

            turn_color = GREEN_LEAF;
            turn_stage = 1;
            turns_remaining[GREEN_LEAF] = TURNS_PER_PLAYER;
            turns_remaining[RED_LEAF] = TURNS_PER_PLAYER;
            game_finished = false;
            winner = 0;
        }
        if (IsKeyPressed(KEY_THREE)) {
            InitBoard(board, 3);
            AddArrows(board, ARROWS_ALL);

            turn_color = GREEN_LEAF;
            turn_stage = 1;
            turns_remaining[GREEN_LEAF] = TURNS_PER_PLAYER;
            turns_remaining[RED_LEAF] = TURNS_PER_PLAYER;
            game_finished = false;
            winner = 0;
        }
        if (IsKeyPressed(KEY_FOUR)) {
            InitBoard(board, 4);
            AddArrows(board, ARROWS_ALL);

            turn_color = GREEN_LEAF;
            turn_stage = 1;
            turns_remaining[GREEN_LEAF] = TURNS_PER_PLAYER;
            turns_remaining[RED_LEAF] = TURNS_PER_PLAYER;
            game_finished = false;
            winner = 0;
        }

        // Zoom and mouse movement
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f/camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }

        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
            camera.offset = GetMousePosition();
            camera.target = mouseWorldPos;
            float scaleFactor = 1.0f + (0.25f*fabsf(wheel));
            if (wheel < 0) scaleFactor = 1.0f/scaleFactor;
            camera.zoom = Clamp(camera.zoom*scaleFactor, 0.125f, 64.0f);
        }

        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(DARKGRAY);

            BeginMode2D(camera);
                int posX = grid_origin.x;
                int posY = grid_origin.y;
                // Draw board tiles
                for (int row = row_min - 1; row <= row_max + 1; row++) {
                    for (int col = col_min - 1; col <= col_max + 1; col++) {
                        switch (board[row][col]) {
                            case GREEN_LEAF:
                                DrawTexture(green_LEAF_texture, posX, posY, WHITE);
                                break;
                            case RED_LEAF:
                                DrawTexture(red_LEAF_texture, posX, posY, WHITE);
                                break;
                            case BRANCH:
                                DrawTexture(branch_texture, posX, posY, WHITE);
                                break;
                            case ARROW_UP:
                                DrawTexture(arrow_up_texture, posX, posY, turn_color == GREEN_LEAF ? GREEN : RED);
                                break;
                            case ARROW_DOWN:
                                DrawTexture(arrow_down_texture, posX, posY, turn_color == GREEN_LEAF ? GREEN : RED);
                                break;    
                            case ARROW_LEFT:
                                DrawTexture(arrow_left_texture, posX, posY, turn_color == GREEN_LEAF ? GREEN : RED);
                                break;  
                            case ARROW_RIGHT:
                                DrawTexture(arrow_right_texture, posX, posY, turn_color == GREEN_LEAF ? GREEN : RED);
                                break;                  
                        }
                        posX += TILE_SIZE;
                    }
                    posY += TILE_SIZE;
                    posX = grid_origin.x;
                }

                mouse = GetScreenToWorld2D(GetMousePosition(), camera);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !game_finished) {
                    selected_col = (mouse.x - grid_origin.x) / TILE_SIZE + col_min - 1;
                    selected_row = (mouse.y - grid_origin.y) / TILE_SIZE + row_min - 1;                    

                    switch (board[selected_row][selected_col]) {
                        case ARROW_LEFT:
                            GameMove(board, turn_color, ARROW_LEFT, selected_col, selected_row);
                            AddArrows(board, ARROW_UP);
                            turn_stage--;
                            turns_remaining[turn_color]--;
                            break;                            
                        case ARROW_RIGHT:
                            GameMove(board, turn_color, ARROW_RIGHT, selected_col, selected_row);
                            AddArrows(board, ARROW_DOWN);
                            turn_stage--;
                            turns_remaining[turn_color]--;
                            break;                        
                        case ARROW_UP:
                            GameMove(board, turn_color, ARROW_UP, selected_col, selected_row);
                            AddArrows(board, ARROW_RIGHT);
                            turn_stage--;
                            turns_remaining[turn_color]--;
                            break;                        
                        case ARROW_DOWN:
                            GameMove(board, turn_color, ARROW_DOWN, selected_col, selected_row);
                            AddArrows(board, ARROW_LEFT);
                            turn_stage--;
                            turns_remaining[turn_color]--;
                            break;
                    }

                    

                    if (turn_color == GREEN_LEAF && turn_stage == 0 && turns_remaining[GREEN_LEAF] > 0) {
                        turn_color = RED_LEAF;
                        turn_stage = 2;
                        AddArrows(board, ARROWS_ALL);
                    }
                    else if (turn_color == RED_LEAF && turn_stage == 0 && turns_remaining[RED_LEAF] > 0) {
                        turn_color = GREEN_LEAF;
                        turn_stage = 2;
                        AddArrows(board, ARROWS_ALL);
                    }
                    else if (turn_color == RED_LEAF && turns_remaining[RED_LEAF] == 0) {
                        turn_color = GREEN_LEAF;
                        turn_stage = 1;
                        AddArrows(board, ARROWS_ALL);
                    }
                    if (turns_remaining[GREEN_LEAF] <= 0 && turns_remaining[RED_LEAF] <= 0) {
                        game_finished = true;
                        AddArrows(board, ARROWS_NONE);
                        winner = CheckWin(board);
                        
                        DrawText(
                        TextFormat("%d, %d", grid_origin.x, row_max - row_min + 1 ),
                        0,
                        0,
                        20, YELLOW);
                    }

                }

                switch (winner) {
                    case GREEN_LEAF:
                        DrawText(
                        TextFormat("ZALIAS LAIMI"), 200, 150, 40, GREEN);
                        break;

                    case RED_LEAF:
                        DrawText(
                        TextFormat("RAUDONAS LAIMI"), 200, 150, 40, RED);
                        break;

                    case 3:
                        DrawText(
                        TextFormat("LYGIOSIOS" ), 200, 150, 40, YELLOW);
                        break;
                    }

            EndMode2D();

        EndDrawing();
    }

    CloseWindow();


    return 0;
}