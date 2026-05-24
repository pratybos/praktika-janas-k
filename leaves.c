#include <raylib.h>
#include "rlgl.h"
#include "raymath.h"

#include <string.h>
#include <time.h>

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

struct Board_Bounds {
    int row_min;
    int row_max;
    int col_min;
    int col_max;
    Vector2 grid_origin;
};

struct Player {
    int color;
    int turns_remaining;
    int moves_remaining;
    enum CpuControlled {PLAYER, CPU_EASY, CPU_MEDIUM} cpu_controlled;
};

struct Board_Bounds InitBoard(int arr[MAX][MAX], int type) {
    struct Board_Bounds bounds;

    for (int row = 0; row < MAX; row++) {
        for (int col = 0; col < MAX; col++) {
            arr[row][col] = 0;
        }
    }
    bounds.row_min = bounds.row_max = bounds.col_min = bounds.col_max = CENTER;

    switch (type) {
        case 1:
            // Vertical line start
            bounds.row_min = CENTER - BRANCH_AMOUNT / 2;
            bounds.row_max = CENTER + BRANCH_AMOUNT / 2;
            bounds.col_min = bounds.col_max = CENTER;  

            for (int i = bounds.row_min; i <= bounds.row_max; i++) {
                arr[i][CENTER] = BRANCH;
            }
            break;

        case 2:
            // Horizontal start
            bounds.col_min = CENTER - BRANCH_AMOUNT / 2;
            bounds.col_max = CENTER + BRANCH_AMOUNT / 2;
            bounds.row_min = bounds.row_max = CENTER;  

            for (int i = bounds.col_min; i <= bounds.col_max; i++) {
                arr[CENTER][i] = BRANCH;
            }
            break;

        case 3:
            // Plus start
            arr[CENTER][CENTER] = 3;    // Center
            for (int i = 1; i < BRANCH_AMOUNT; i++) {
                switch (i % 4) {
                    case 1:         // Left
                        bounds.col_min--;
                        arr[CENTER][bounds.col_min] = BRANCH;
                        break;
                    case 2:         // UP
                        bounds.row_min--;
                        arr[bounds.row_min][CENTER] = BRANCH;
                        break;
                    case 3:         // RIGHT
                        bounds.col_max++;
                        arr[CENTER][bounds.col_max] = BRANCH;
                        break;
                    case 0:         // DOWN
                        bounds.row_max++;
                        arr[bounds.row_max][CENTER] = BRANCH;
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
                        bounds.col_min--;
                        arr[bounds.row_min][bounds.col_min] = BRANCH;
                        break;
                    case 2:         // Right
                        bounds.col_max++;
                        arr[bounds.row_min][bounds.col_max] = BRANCH;
                        break;
                    case 3:         // LEFT-UP
                        bounds.row_min--;
                        arr[bounds.row_min][bounds.col_min] = BRANCH;
                        break;
                    case 0:         // RIGHT-UP
                        arr[bounds.row_min][bounds.col_max] = BRANCH;
                        break;
                }
            }
            break;
    }

    int grid_width = (bounds.col_max - bounds.col_min + 3) * TILE_SIZE;
    int grid_height = (bounds.row_max - bounds.row_min + 3) * TILE_SIZE; 
    bounds.grid_origin.x = (SCREEN_WIDTH - grid_width) / 2;
    bounds.grid_origin.y = (SCREEN_HEIGHT - grid_height) / 2;

    return bounds;
}

void AddArrows(int arr[MAX][MAX], struct Board_Bounds bounds, int mode) {

        // Clear arrows
        for (int row = bounds.row_min - 1; row <= bounds.row_max + 1; row++)
            for (int col = bounds.col_min - 1; col <= bounds.col_max + 1; col++) 
                if (arr[row][col] == ARROW_LEFT || 
                    arr[row][col] == ARROW_RIGHT || 
                    arr[row][col] == ARROW_UP || 
                    arr[row][col] == ARROW_DOWN )
                        arr[row][col] = 0;
        
        if (mode == ARROWS_NONE)
            return;

        for (int i = bounds.col_min; i <= bounds.col_max; i++) {
            if (mode == ARROW_DOWN || mode == ARROWS_ALL)
                arr[bounds.row_min - 1][i] = ARROW_DOWN; 
            if (mode == ARROW_UP || mode == ARROWS_ALL)
                arr[bounds.row_max + 1][i] = ARROW_UP;
        }

        for (int i = bounds.row_min; i <= bounds.row_max; i++) {
            if (mode == ARROW_LEFT || mode == ARROWS_ALL) 
                arr[i][bounds.col_max + 1] = ARROW_LEFT;
            if (mode == ARROW_RIGHT || mode == ARROWS_ALL)
                arr[i][bounds.col_min - 1] = ARROW_RIGHT;
        }        
}

struct Board_Bounds GameMove(int arr[MAX][MAX], struct Board_Bounds bounds, int turn_color, int direction, int col, int row) {
    // Tile pushing based on direction
    int i, j;
    switch (direction) {

        case ARROW_LEFT:
            i = bounds.col_max;
            while (arr[row][i] == EMPTY_SPACE || arr[row][i] > 3)
                i--;

            j = i - 1;
            while (arr[row][j] == RED_LEAF || arr[row][j] == GREEN_LEAF || arr[row][j] == BRANCH)
                j--;

            if (j < bounds.col_min) {
                bounds.col_min--;
                bounds.grid_origin.x -= TILE_SIZE;
            }

            for (; j <= i; j++)
                arr[row][j] = arr[row][j + 1];
            
            arr[row][i] = turn_color;
            break;

        case ARROW_RIGHT:
            i = bounds.col_min;
            while (arr[row][i] == EMPTY_SPACE || arr[row][i] > 3)
                i++;

            j = i + 1;
            while (arr[row][j] == RED_LEAF || arr[row][j] == GREEN_LEAF || arr[row][j] == BRANCH)
                j++;

            if (j > bounds.col_max) {
                bounds.col_max++;
            }

            for (; j >= i; j--)
                arr[row][j] = arr[row][j - 1];

            arr[row][i] = turn_color;
            break;
            
        case ARROW_UP:
            i = bounds.row_max;
            while (arr[i][col] == EMPTY_SPACE || arr[i][col] > 3)
                i--;

            j = i - 1;
            while (arr[j][col] == RED_LEAF || arr[j][col] == GREEN_LEAF || arr[j][col]== BRANCH)
                j--;

            if (j < bounds.row_min) {
                bounds.row_min--;
                bounds.grid_origin.y -= TILE_SIZE;
            }

            for (; j <= i; j++)
                arr[j][col]= arr[j + 1][col];
            
            arr[i][col] = turn_color;
            break;

        case ARROW_DOWN:
            i = bounds.row_min;
            while (arr[i][col] == EMPTY_SPACE || arr[i][col] > 3)
                i++;

            j = i + 1;
            while (arr[j][col] == RED_LEAF || arr[j][col] == GREEN_LEAF || arr[j][col]== BRANCH)
                j++;

            if (j > bounds.row_max) {
                bounds.row_max++;
            }

            for (; j >= i; j--)
                arr[j][col]= arr[j - 1][col];
            
            arr[i][col] = turn_color;
            break;
    }

    return bounds;
}

struct Score {
    int green;
    int red;
};

struct Score CheckScore(int arr[MAX][MAX], struct Board_Bounds bounds) {

    struct Score score;
    score.green = 0;
    score.red = 0;
    
    for (int row = bounds.row_min; row <= bounds.row_max; row++) {
        for (int col = bounds.col_min; col <= bounds.col_max; col++) {
            if (arr[row + 1][col] == BRANCH || 
                arr[row - 1][col] == BRANCH || 
                arr[row][col + 1] == BRANCH || 
                arr[row][col - 1] == BRANCH) {
                    if (arr[row][col] == GREEN_LEAF)
                        score.green += 1;
                    else if (arr[row][col] == RED_LEAF)
                        score.red += 1;
                }
            }
        }

    return score;        
}


struct Moves {
    int row;
    int col;
    int direction;
    int score;
    struct Moves* next;
};
struct Moves* head;

struct Final_Move {
    int row;
    int col;
    int direction;
};

struct Moves* InsertToBack(struct Moves* head, int direction, int row, int col) {
    struct Moves* temp = (struct Moves*)MemAlloc(sizeof(struct Moves));
    temp->row = row;
    temp->col = col;
    temp->direction = direction;
    temp->score = 0;
    temp->next = NULL;
    if (head == NULL)
        head = temp;
    else {
        struct Moves* temp1 = head;
        while (temp1->next != NULL)
            temp1 = temp1->next;
        temp1->next = temp;
    }
    return head;
}

struct Moves* destroy(struct Moves* head) {
    struct Moves *current = head;
    struct Moves *temp;

    while (current != NULL) {
        temp = current->next;
        MemFree(current);
        current = temp;
    }
    return NULL;
}

struct Final_Move CpuMoves(int board[MAX][MAX], struct Board_Bounds bounds, int theory_board[MAX][MAX], int turn_color, struct Moves* head, int cpu_difficulty) { 
    /*
    1. Trinamas susietas ejimu sarasas (Moves)
    2. Visi galimi ejimai surasomi i susieta sarasa
    3. Ciklas:
        Laikinas lentos masyvas kopijuojamas is pagrindinio masyvo board
        Ejimas is saraso atliekamas nukopijuotame masyve
        Skaiciuojamas tasku pokytis pries ir po ejimo
        Issaugomas didziausias tasku pokytis i max_score
    4. Is susieto saraso Moves trinami ejimai kuriu verte mazesne uz max_score
    5. Is likusiu ejimu sarase pasirenkamas atsitiktinis
    6. Pasirinktas ejimas pritaikomas pagrindiniam masyvui "board"
    */

    head = destroy(head);

    struct Board_Bounds theory_bounds = bounds;

    int list_length = 0;
    for (int row = theory_bounds.row_min - 1; row <= theory_bounds.row_max + 1; row++) {
        for (int col = theory_bounds.col_min - 1; col <= theory_bounds.col_max + 1; col++) {
            if (board[row][col] == ARROW_LEFT || 
                board[row][col] == ARROW_RIGHT || 
                board[row][col] == ARROW_UP || 
                board[row][col] == ARROW_DOWN ) {
                    head = InsertToBack(head, board[row][col], row, col);
                    list_length++;
            }
        }
    }
    
    struct Moves* current = head, *prev;
    struct Score score_after;
    
    int max_score = -TILES_PER_PLAYER;
    while (current != NULL) {
        memcpy(theory_board, board, sizeof(board[0][0]) * MAX * MAX); 
        GameMove(theory_board, theory_bounds, turn_color, current->direction, current->col, current->row);
        score_after = CheckScore(theory_board, theory_bounds);
        if (turn_color == RED_LEAF)
            current->score = score_after.red - score_after.green;
        else
            current->score = score_after.green - score_after.red;
            
        if (current->score > max_score)
            max_score = current->score;

        current = current->next;
        theory_bounds = bounds;
    }

    if (cpu_difficulty == 1)
        max_score -= 2;

    current = head;
    prev = head;
    while (current != NULL) {
        if (current->score < max_score) {
            if (current == head) {
                head = current->next;
                MemFree(current);
                current = head;
                prev = head;
                list_length--;
            }
            else {
                prev->next = current->next;
                MemFree(current);
                current = prev->next;
                list_length--;
            }
        }
        else {
            prev = current;
            current = current->next;
        }
    }    
     
    SetRandomSeed(time(NULL));
    int random_move = GetRandomValue(1, list_length);
    
    current = head;
    for (int n = 1; n < random_move; n++)
        current = current->next;

    struct Final_Move result;
    result.row = current->row;
    result.col = current->col;
    result.direction = current->direction;

    return result;
}

int main(void) {
    int board[MAX][MAX];       
    
    struct Board_Bounds bounds;
    bounds = InitBoard(board, 1);
    AddArrows(board, bounds, ARROWS_ALL);

    // SetConfigFlags(FLAG_MSAA_4X_HINT);
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
    bool game_finished = false;
    int theory_board[MAX][MAX];
    int player_num = 0;
    bool move_made = false;
    
    struct Score score;
    struct Final_Move cpu_move;

    struct Player player[2] = {
        {GREEN_LEAF, TURNS_PER_PLAYER, 1, PLAYER}, 
        {RED_LEAF, TURNS_PER_PLAYER, 0, CPU_EASY}
    };

    while (!WindowShouldClose()) {

        // RESET BOARD
        if (IsKeyPressed(KEY_ONE)) {
            bounds = InitBoard(board, 1);
        }
        if (IsKeyPressed(KEY_TWO)) {
            bounds = InitBoard(board, 2);
        }
        if (IsKeyPressed(KEY_THREE)) {
            bounds = InitBoard(board, 3);
        }
        if (IsKeyPressed(KEY_FOUR)) {
            bounds = InitBoard(board, 4);
        }

        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_TWO) || 
            IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_FOUR)) {

            AddArrows(board, bounds, ARROWS_ALL);
            player[0].moves_remaining = 1;
            player[1].moves_remaining = 0;
            player[0].turns_remaining = TURNS_PER_PLAYER;
            player[1].turns_remaining = TURNS_PER_PLAYER;
            game_finished = false;
            score.green = 0;
            score.red = 0;
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
            camera.zoom = Clamp(camera.zoom*scaleFactor, 0.3f, 3.0f);
        }

        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(DARKGRAY);

            BeginMode2D(camera);
                int posX = bounds.grid_origin.x;
                int posY = bounds.grid_origin.y;
                // Draw board tiles
                for (int row = bounds.row_min - 1; row <= bounds.row_max + 1; row++) {
                    for (int col = bounds.col_min - 1; col <= bounds.col_max + 1; col++) {
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
                                DrawTexture(arrow_up_texture, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                break;
                            case ARROW_DOWN:
                                DrawTexture(arrow_down_texture, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                break;    
                            case ARROW_LEFT:
                                DrawTexture(arrow_left_texture, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                break;  
                            case ARROW_RIGHT:
                                DrawTexture(arrow_right_texture, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                break;                  
                        }
                        posX += TILE_SIZE;
                    }
                    posY += TILE_SIZE;
                    posX = bounds.grid_origin.x;
                }

                if (player[player_num].cpu_controlled && !game_finished) {
                    cpu_move = CpuMoves(board, bounds, theory_board, player[player_num].color, head, player[player_num].cpu_controlled);
                    bounds = GameMove(board, bounds, player[player_num].color, cpu_move.direction, cpu_move.col, cpu_move.row);

                    switch (cpu_move.direction) {
                        case ARROW_LEFT:
                            AddArrows(board, bounds, ARROW_UP);
                            break;                            
                        case ARROW_RIGHT:
                            AddArrows(board, bounds, ARROW_DOWN);
                            break;                        
                        case ARROW_UP:
                            AddArrows(board, bounds, ARROW_RIGHT);
                            break;                        
                        case ARROW_DOWN:
                            AddArrows(board, bounds, ARROW_LEFT);
                            break;
                    }
                    move_made = true;                    
                }

                mouse = GetScreenToWorld2D(GetMousePosition(), camera);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !game_finished) {
                    selected_col = (mouse.x - bounds.grid_origin.x) / TILE_SIZE + bounds.col_min - 1;
                    selected_row = (mouse.y - bounds.grid_origin.y) / TILE_SIZE + bounds.row_min - 1;  

                    int selected_square = board[selected_row][selected_col];

                    switch (selected_square) {
                        case ARROW_LEFT:
                            bounds = GameMove(board, bounds, player[player_num].color, ARROW_LEFT, selected_col, selected_row);
                            AddArrows(board, bounds, ARROW_UP);
                            break;                            
                        case ARROW_RIGHT:
                            bounds = GameMove(board, bounds, player[player_num].color, ARROW_RIGHT, selected_col, selected_row);
                            AddArrows(board, bounds, ARROW_DOWN);
                            break;                        
                        case ARROW_UP:
                            bounds = GameMove(board, bounds, player[player_num].color, ARROW_UP, selected_col, selected_row);
                            AddArrows(board, bounds, ARROW_RIGHT);
                            break;                        
                        case ARROW_DOWN:
                            bounds = GameMove(board, bounds, player[player_num].color, ARROW_DOWN, selected_col, selected_row);
                            AddArrows(board, bounds, ARROW_LEFT);
                            break;
                    }

                    if (selected_square == ARROW_LEFT || selected_square == ARROW_RIGHT || 
                        selected_square == ARROW_UP || selected_square == ARROW_DOWN) {
                            move_made = true;
                    }
                }
                
                if (move_made) {
                    player[player_num].moves_remaining--;
                    player[player_num].turns_remaining--;
                    score = CheckScore(board, bounds);

                    if (player[player_num].moves_remaining == 0) {
                        player_num = (player_num + 1) % (sizeof(player) / sizeof(player[0]));
                        player[player_num].moves_remaining = 2;
                        AddArrows(board, bounds, ARROWS_ALL);
                    }
   
                    if (player[0].turns_remaining == 0 && player[1].turns_remaining == 0) {
                        game_finished = true;
                        AddArrows(board, bounds, ARROWS_NONE);
                    }

                    move_made = false;
                }

            EndMode2D();

            if (game_finished) {
                if (score.green > score.red)
                    DrawText(TextFormat("ZALIAS LAIMI"), 20, 20, 40, GREEN);
                else if (score.green < score.red)
                    DrawText(TextFormat("RAUDONAS LAIMI"), 20, 20, 40, RED);
                else
                    DrawText(TextFormat("LYGIOSIOS"), 20, 20, 40, YELLOW);

                DrawText(TextFormat("   %d", score.green), 20, 60, 50, GREEN);
                DrawText(TextFormat("     -"), 20, 60, 50, WHITE);
                DrawText(TextFormat("       %d", score.red), 20, 60, 50, RED);
            }
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
